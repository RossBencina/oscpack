/*
	oscpack -- Open Sound Control packet manipulation library
	http://www.audiomulch.com/~rossb/oscpack
	
	Copyright (c) 2004 Ross Bencina <rossb@audiomulch.com>
	
	Permission is hereby granted, free of charge, to any person obtaining
	a copy of this software and associated documentation files
	(the "Software"), to deal in the Software without restriction,
	including without limitation the rights to use, copy, modify, merge,
	publish, distribute, sublicense, and/or sell copies of the Software,
	and to permit persons to whom the Software is furnished to do so,
	subject to the following conditions:
	
	The above copyright notice and this permission notice shall be
	included in all copies or substantial portions of the Software.
	
	Any person wishing to distribute modifications to the Software is
	requested to send the modifications to the original developer so that
	they can be incorporated into the canonical version.
	
	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
	EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
	MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
	IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
	ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
	CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
	WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "UdpPacketListenerPort.h"

#include <stdexcept>

#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h> // for sockaddr_in


class UdpPacketListenerPort::Implementation{
    enum { MAX_PACKET_SIZE = 1024 * 10 }; // only fixed for udp messaging

    int portNumber_;
    UdpPacketListener *listener_;
    int socket_;
    pthread_t thread_;
    char *data_;

    static void* ThreadFunc( void* p )
    {
        static_cast<UdpPacketListenerPort::Implementation*>(p)->ThreadTask();
        return 0;
    };

    void ThreadTask()
    {
        struct sockaddr replyAddr;
        socklen_t replyAddrLength;

        while (true) {
	    
            replyAddrLength = sizeof(sockaddr_in);
            int size = recvfrom(socket_, data_, MAX_PACKET_SIZE , 0,
                                    (struct sockaddr *) &replyAddr, (socklen_t*)&replyAddrLength);

            if (size > 0){
                listener_->ProcessPacket( data_, size );
            }else{
                break;
            }
        }
    };

    void Start()
    {
        if ((socket_ = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
            throw std::runtime_error("failed to create udp socket\n");
        }

        struct sockaddr_in bindSockAddr;

        bzero((char *)&bindSockAddr, sizeof(bindSockAddr));
        bindSockAddr.sin_family = AF_INET;
        bindSockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
        bindSockAddr.sin_port = htons(portNumber_);

        if (bind(socket_, (struct sockaddr *)&bindSockAddr, sizeof(bindSockAddr)) < 0) {
            throw std::runtime_error("unable to bind udp socket\n");
        }
        
        int threadId = pthread_create(&thread_ , NULL, ThreadFunc, this);
    };

    void Stop()
    {
        if( thread_ ){
   	    
	    /* fixme : stop thread here*/
            thread_ = 0;

             if (socket_ != -1) close(socket_);
        }
    };
    
public:

    Implementation( int portNumber, UdpPacketListener *listener )
        : portNumber_( portNumber )
        , listener_( listener )
        , socket_( -1 )
        , thread_( 0 )
        , data_( new char[ MAX_PACKET_SIZE ] )
    {
        Start();
    };

    ~Implementation()
    {
        Stop();

        delete [] data_;
    };
};

UdpPacketListenerPort::UdpPacketListenerPort( int portNumber, UdpPacketListener *listener )
{
    impl_ = new Implementation( portNumber, listener );
}

UdpPacketListenerPort::~UdpPacketListenerPort()
{
    delete impl_;
}
