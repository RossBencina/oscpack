#include "UdpSocket.h"

#ifdef WIN32
#include <winsock2.h>   // this must come first to prevent errors with MSVC7
#include <windows.h>
#include <mmsystem.h> // for timeGetTime()

typedef int socklen_t;
#endif

#include <vector>
#include <algorithm>
#include <stdexcept>
#include <assert.h>
#include <signal.h>
#include <math.h>
#include <errno.h>
#include <string.h> // for memset

#ifndef WIN32
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h> // for sockaddr_in
#endif

#include "PacketListener.h"
#include "TimerListener.h"


// not sure why the following is missing on OSX
#ifdef __APPLE__
typedef ssize_t socklen_t;
#endif


static void SockaddrFromIpEndpointName( struct sockaddr_in& sockAddr, const IpEndpointName& endpoint )
{
    memset( (char *)&sockAddr, 0, sizeof(sockAddr ) );
    sockAddr.sin_family = AF_INET;

	sockAddr.sin_addr.s_addr = 
		(endpoint.address == IpEndpointName::ANY_ADDRESS)
		? INADDR_ANY
		: htonl( endpoint.address );

	sockAddr.sin_port =
		(endpoint.port == IpEndpointName::ANY_PORT)
		? 0
		: htons( endpoint.port );
}


static IpEndpointName IpEndpointNameFromSockaddr( const struct sockaddr_in& sockAddr )
{
	return IpEndpointName( 
		(sockAddr.sin_addr.s_addr == INADDR_ANY) 
			? IpEndpointName::ANY_ADDRESS 
			: ntohl( sockAddr.sin_addr.s_addr ),
		(sockAddr.sin_port == 0)
			? IpEndpointName::ANY_PORT
			: ntohs( sockAddr.sin_port )
		);
}


class UdpSocket::Implementation{
	bool isBound_;
	bool isConnected_;

	int socket_;
	struct sockaddr_in connectedAddr_;
	struct sockaddr_in sendToAddr_;

public:

	Implementation()
		: isBound_( false )
		, isConnected_( false )
		, socket_( -1 )
	{
		if( (socket_ = socket( AF_INET, SOCK_DGRAM, 0 )) == -1 ){
            throw std::runtime_error("unable to create udp socket\n");
        }

		memset( &sendToAddr_, 0, sizeof(sendToAddr_) );
        sendToAddr_.sin_family = AF_INET;
	}

	~Implementation()
	{
#ifdef WIN32
		if (socket_ != -1) closesocket(socket_);
#else
		if (socket_ != -1) close(socket_);
#endif
	}

	IpEndpointName LocalEndpointFor( const IpEndpointName& remoteEndpoint ) const
	{
		assert( isBound_ );

		// first connect the socket to the remote server
        
        struct sockaddr_in connectSockAddr;
		SockaddrFromIpEndpointName( connectSockAddr, remoteEndpoint );
       
        if (connect(socket_, (struct sockaddr *)&connectSockAddr, sizeof(connectSockAddr)) < 0) {
            throw std::runtime_error("unable to connect udp socket\n");
        }

        // get the address

        struct sockaddr_in sockAddr;
        memset( (char *)&sockAddr, 0, sizeof(sockAddr ) );
        socklen_t length = sizeof(sockAddr);
        if (getsockname(socket_, (struct sockaddr *)&sockAddr, &length) < 0) {
            throw std::runtime_error("unable to getsockname\n");
        }
        
		if( isConnected_ ){
			// reconnect to the connected address
			
			if (connect(socket_, (struct sockaddr *)&connectedAddr_, sizeof(connectedAddr_)) < 0) {
				throw std::runtime_error("unable to connect udp socket\n");
			}

		}else{
			// unconnect from the remote address
		
#ifdef WIN32
			struct sockaddr_in unconnectSockAddr;
			SockaddrFromIpEndpointName( unconnectSockAddr, IpEndpointName() );

			if( connect(socket_, (struct sockaddr *)&unconnectSockAddr, sizeof(unconnectSockAddr)) < 0 
					&& WSAGetLastError() != WSAEADDRNOTAVAIL ){
				throw std::runtime_error("unable to un-connect udp socket\n");
			}
#else
			struct sockaddr_in unconnectSockAddr;
			memset( (char *)&unconnectSockAddr, 0, sizeof(unconnectSockAddr ) );
			unconnectSockAddr.sin_family = AF_UNSPEC;
			// address fields are zero
			int connectResult = connect(socket_, (struct sockaddr *)&unconnectSockAddr, sizeof(unconnectSockAddr));
			if ( connectResult < 0 && errno != EAFNOSUPPORT ) {
				throw std::runtime_error("unable to un-connect udp socket\n");
			}
#endif
		}

		return IpEndpointNameFromSockaddr( sockAddr );
	}

	void Connect( const IpEndpointName& remoteEndpoint )
	{
		SockaddrFromIpEndpointName( connectedAddr_, remoteEndpoint );
       
        if (connect(socket_, (struct sockaddr *)&connectedAddr_, sizeof(connectedAddr_)) < 0) {
            throw std::runtime_error("unable to connect udp socket\n");
        }

		isConnected_ = true;
	}

	void Send( const char *data, int size )
	{
		assert( isConnected_ );

        send( socket_, data, size, 0 );
	}

    void SendTo( const IpEndpointName& remoteEndpoint, const char *data, int size )
	{
		sendToAddr_.sin_addr.s_addr = htonl( remoteEndpoint.address );
        sendToAddr_.sin_port = htons( remoteEndpoint.port );

        sendto( socket_, data, size, 0, (sockaddr*)&sendToAddr_, sizeof(sendToAddr_) );
	}

	void Bind( const IpEndpointName& localEndpoint )
	{
		struct sockaddr_in bindSockAddr;
		SockaddrFromIpEndpointName( bindSockAddr, localEndpoint );

        if (bind(socket_, (struct sockaddr *)&bindSockAddr, sizeof(bindSockAddr)) < 0) {
            throw std::runtime_error("unable to bind udp socket\n");
        }

		isBound_ = true;
	}

	bool IsBound() const { return isBound_; }

    int ReceiveFrom( IpEndpointName& remoteEndpoint, char *data, int size )
	{
		assert( isBound_ );

		struct sockaddr_in fromAddr;
        socklen_t fromAddrLen = sizeof(fromAddr);
             	 
        int result = recvfrom(socket_, data, size, 0,
                    (struct sockaddr *) &fromAddr, (socklen_t*)&fromAddrLen);
		if( result < 0 )
			return 0;

		remoteEndpoint.address = ntohl(fromAddr.sin_addr.s_addr);
		remoteEndpoint.port = ntohs(fromAddr.sin_port);

		return result;
	}

	int Socket() { return socket_; }
};

UdpSocket::UdpSocket()
{
	impl_ = new Implementation();
}

UdpSocket::~UdpSocket()
{
	delete impl_;
}

IpEndpointName UdpSocket::LocalEndpointFor( const IpEndpointName& remoteEndpoint ) const
{
	return impl_->LocalEndpointFor( remoteEndpoint );
}

void UdpSocket::Connect( const IpEndpointName& remoteEndpoint )
{
	impl_->Connect( remoteEndpoint );
}

void UdpSocket::Send( const char *data, int size )
{
	impl_->Send( data, size );
}

void UdpSocket::SendTo( const IpEndpointName& remoteEndpoint, const char *data, int size )
{
	impl_->SendTo( remoteEndpoint, data, size );
}

void UdpSocket::Bind( const IpEndpointName& localEndpoint )
{
	impl_->Bind( localEndpoint );
}

bool UdpSocket::IsBound() const
{
	return impl_->IsBound();
}

int UdpSocket::ReceiveFrom( IpEndpointName& remoteEndpoint, char *data, int size )
{
	return impl_->ReceiveFrom( remoteEndpoint, data, size );
}


struct AttachedTimerListener{
	AttachedTimerListener( int id, int p, TimerListener *tl )
		: initialDelayMs( id )
		, periodMs( p )
		, listener( tl ) {}
	int initialDelayMs;
	int periodMs;
	TimerListener *listener;
};


static bool CompareScheduledTimerCalls( 
		const std::pair< double, AttachedTimerListener > & lhs, const std::pair< double, AttachedTimerListener > & rhs )
{
	return lhs.first < rhs.first;
}


SocketReceiveMultiplexer *multiplexerInstanceToAbortWithSigInt_ = 0;

extern "C" static void InterruptSignalHandler( int );
static void InterruptSignalHandler( int )
{
	multiplexerInstanceToAbortWithSigInt_->AsynchronousBreak();
	signal( SIGINT, SIG_DFL );
}


class SocketReceiveMultiplexer::Implementation{
	std::vector< std::pair< PacketListener*, UdpSocket* > > socketListeners_;
	std::vector< AttachedTimerListener > timerListeners_;

	volatile bool break_;

	double GetCurrentTimeMs() const
	{
		struct timeval t;

		gettimeofday( &t, 0 );

		return ((double)t.tv_sec*1000.) + ((double)t.tv_usec / 1000.);
	}

public:
    Implementation()
	{
	}

    ~Implementation()
	{
	}

    void AttachSocketListener( UdpSocket *socket, PacketListener *listener )
	{
		assert( std::find( socketListeners_.begin(), socketListeners_.end(), std::make_pair(listener, socket) ) == socketListeners_.end() );
		// we don't check that the same socket has been added multiple times, even though this is an error
		socketListeners_.push_back( std::make_pair( listener, socket ) );
	}

    void DetachSocketListener( UdpSocket *socket, PacketListener *listener )
	{
		std::vector< std::pair< PacketListener*, UdpSocket* > >::iterator i = 
				std::find( socketListeners_.begin(), socketListeners_.end(), std::make_pair(listener, socket) );
		assert( i != socketListeners_.end() );

		socketListeners_.erase( i );
	}

    void AttachPeriodicTimerListener( int periodMilliseconds, TimerListener *listener )
	{
		timerListeners_.push_back( AttachedTimerListener( periodMilliseconds, periodMilliseconds, listener ) );
	}

	void AttachPeriodicTimerListener( int initialDelayMilliseconds, int periodMilliseconds, TimerListener *listener )
	{
		timerListeners_.push_back( AttachedTimerListener( initialDelayMilliseconds, periodMilliseconds, listener ) );
	}

    void DetachPeriodicTimerListener( TimerListener *listener )
	{
		std::vector< AttachedTimerListener >::iterator i = timerListeners_.begin();
		while( i != timerListeners_.end() ){
			if( i->listener == listener )
				break;
			++i;
		}

		assert( i != timerListeners_.end() );

		timerListeners_.erase( i );
	}

    void Run()
	{
		break_ = false;

		// configure the master fd_set for select()

		fd_set masterfds, tempfds;
		FD_ZERO( &masterfds );
		FD_ZERO( &tempfds );
		int fdmax = 0;

		for( std::vector< std::pair< PacketListener*, UdpSocket* > >::iterator i = socketListeners_.begin();
				i != socketListeners_.end(); ++i ){

			if( fdmax < i->second->impl_->Socket() )
				fdmax = i->second->impl_->Socket();
			FD_SET( i->second->impl_->Socket(), &masterfds );
		}


		// configure the timer queue
		double currentTimeMs = GetCurrentTimeMs();

		// expiry time ms, listener
		std::vector< std::pair< double, AttachedTimerListener > > timerQueue_;
		for( std::vector< AttachedTimerListener >::iterator i = timerListeners_.begin();
				i != timerListeners_.end(); ++i )
			timerQueue_.push_back( std::make_pair( currentTimeMs + i->initialDelayMs, *i ) );
		std::sort( timerQueue_.begin(), timerQueue_.end(), CompareScheduledTimerCalls );

		const int MAX_BUFFER_SIZE = 4098;
		char *data = new char[ MAX_BUFFER_SIZE ];
		IpEndpointName remoteEndpoint;

		struct timeval timeout;

		while( !break_ ){
			tempfds = masterfds;

			struct timeval *timeoutPtr = 0;
			if( !timerQueue_.empty() ){
				double timeoutMs = timerQueue_.front().first - GetCurrentTimeMs();
				if( timeoutMs < 0 )
					timeoutMs = 0;
			
                // 1000000 microseconds in a second
                timeout.tv_sec = (long)(timeoutMs * .001);
                timeout.tv_usec = (long)((timeoutMs - (timeout.tv_sec * 1000)) * 1000);
				timeoutPtr = &timeout;
			}

			if( select( fdmax + 1, &tempfds, 0, 0, timeoutPtr ) < 0 && errno != EINTR ){
   				throw std::runtime_error("select failed\n");
			}
			if( break_ )
				break;

			for( std::vector< std::pair< PacketListener*, UdpSocket* > >::iterator i = socketListeners_.begin();
					i != socketListeners_.end(); ++i ){

				if( FD_ISSET( i->second->impl_->Socket(), &tempfds ) ){

					int size = i->second->ReceiveFrom( remoteEndpoint, data, MAX_BUFFER_SIZE );
					if( size > 0 ){
						i->first->PacketReceived( data, size, remoteEndpoint );
						if( break_ )
							break;
					}
				}
			}


			// execute any expired timers
			currentTimeMs = GetCurrentTimeMs();
			bool resort = false;
			for( std::vector< std::pair< double, AttachedTimerListener > >::iterator i = timerQueue_.begin();
					i != timerQueue_.end() && i->first <= currentTimeMs; ++i ){

				i->second.listener->TimerExpired();
				if( break_ )
					break;

				i->first += i->second.periodMs;
				resort = true;
			}
			if( resort )
				std::sort( timerQueue_.begin(), timerQueue_.end(), CompareScheduledTimerCalls );
		}

		delete [] data;
	}

    void Break()
	{
		break_ = true;
	}

    void AsynchronousBreak()
	{
		break_ = true;
		// FIXME: need to do something here to break through the select, not sure what
		// for now single threaded apps will work fine because ct
	}
};



SocketReceiveMultiplexer::SocketReceiveMultiplexer()
{
	impl_ = new Implementation();
}

SocketReceiveMultiplexer::~SocketReceiveMultiplexer()
{	
	delete impl_;
}

void SocketReceiveMultiplexer::AttachSocketListener( UdpSocket *socket, PacketListener *listener )
{
	impl_->AttachSocketListener( socket, listener );
}

void SocketReceiveMultiplexer::DetachSocketListener( UdpSocket *socket, PacketListener *listener )
{
	impl_->DetachSocketListener( socket, listener );
}

void SocketReceiveMultiplexer::AttachPeriodicTimerListener( int periodMilliseconds, TimerListener *listener )
{
	impl_->AttachPeriodicTimerListener( periodMilliseconds, listener );
}

void SocketReceiveMultiplexer::AttachPeriodicTimerListener( int initialDelayMilliseconds, int periodMilliseconds, TimerListener *listener )
{
	impl_->AttachPeriodicTimerListener( initialDelayMilliseconds, periodMilliseconds, listener );
}

void SocketReceiveMultiplexer::DetachPeriodicTimerListener( TimerListener *listener )
{
	impl_->DetachPeriodicTimerListener( listener );
}

void SocketReceiveMultiplexer::Run()
{
	impl_->Run();
}

void SocketReceiveMultiplexer::RunUntilSigInt()
{
	assert( multiplexerInstanceToAbortWithSigInt_ == 0 );
	multiplexerInstanceToAbortWithSigInt_ = this;
	signal( SIGINT, InterruptSignalHandler );
	impl_->Run();
}

void SocketReceiveMultiplexer::Break()
{
	impl_->Break();
}

void SocketReceiveMultiplexer::AsynchronousBreak()
{
	impl_->AsynchronousBreak();
}

