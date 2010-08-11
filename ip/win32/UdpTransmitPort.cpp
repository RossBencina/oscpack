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
#include "UdpTransmitPort.h"

#include <winsock2.h>   // this must come first to prevent errors with MSVC7
#include <stdexcept>

class UdpTransmitPort::Implementation{

    struct sockaddr_in serveraddr_;
    int socket_;
    
public:
    Implementation( unsigned long address, int port )
    {
        memset( &serveraddr_, 0, sizeof(serveraddr_) );
        serveraddr_.sin_family = AF_INET;
        serveraddr_.sin_addr.S_un.S_addr = address;
        serveraddr_.sin_port = htons( (short)port );
        if( (socket_ = socket( AF_INET, SOCK_DGRAM, 0 )) < 0 ){
            throw std::runtime_error("unable to create udp socket\n");
        }
    }

    ~Implementation()
    {
        if (socket_ != -1) closesocket(socket_);
    }

    void Send( const char *p, int count )
    {
        sendto( socket_, p, count, 0, (sockaddr*)&serveraddr_, sizeof(serveraddr_) );
    }
};


UdpTransmitPort::UdpTransmitPort( unsigned long address, int port )
{
    impl_ = new Implementation( address, port );
}

UdpTransmitPort::~UdpTransmitPort()
{
    delete impl_;
}

void UdpTransmitPort::Send( const char *p, int count )
{
    impl_->Send( p, count );
}
