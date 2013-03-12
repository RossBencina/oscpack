/*
	oscpack -- Open Sound Control (OSC) packet manipulation library
	http://www.rossbencina.com/code/oscpack

	Copyright (c) 2004-2013 Ross Bencina <rossb@audiomulch.com>

	Permission is hereby granted, free of charge, to any person obtaining
	a copy of this software and associated documentation files
	(the "Software"), to deal in the Software without restriction,
	including without limitation the rights to use, copy, modify, merge,
	publish, distribute, sublicense, and/or sell copies of the Software,
	and to permit persons to whom the Software is furnished to do so,
	subject to the following conditions:

	The above copyright notice and this permission notice shall be
	included in all copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
	EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
	MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
	IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
	ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
	CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
	WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

/*
	The text above constitutes the entire oscpack license; however, 
	the oscpack developer(s) also make the following non-binding requests:

	Any person wishing to distribute modifications to the Software is
	requested to send the modifications to the original developer so that
	they can be incorporated into the canonical version. It is also 
	requested that these non-binding requests be included whenever the
	above license is reproduced.
*/
#include "ip/NetworkingUtils.h"

#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include <cassert>
#include <cstring>


NetworkInitializer::NetworkInitializer() {}

NetworkInitializer::~NetworkInitializer() {}


/*
unsigned long GetHostByName( const char *name )
{
    unsigned long result = 0;

    struct hostent *h = gethostbyname( name );
    if( h ){
        struct in_addr a;
        std::memcpy( &a, h->h_addr_list[0], h->h_length );
        result = ntohl(a.s_addr);
    }

    return result;
}
*/

void IpAddressFromString( unsigned char *address16Chars, unsigned long *scopeZoneIndex, bool *isIpV6Address, const char *addressString )
{
    struct addrinfo *ai;
    struct addrinfo hints;
    std::memset( &hints, 0, sizeof(hints) );
    hints.ai_family = PF_UNSPEC;
    hints.ai_flags = 0; //AI_NUMERICHOST;
    int errcode = getaddrinfo( addressString, NULL, &hints, &ai );
    if( errcode != 0 )
        assert( errcode == 0 );
    
    if( ai->ai_addr->sa_family == AF_INET ){
        struct sockaddr_in *sin = (struct sockaddr_in*)ai->ai_addr;
        
        std::memset( address16Chars, 0, 10 );
        
        // ipv4 adresses are stored in ::ffff:0:0/96 form
        
        address16Chars[10] = 0xFF;
        address16Chars[11] = 0xFF;
        
        in_addr_t addr = ntohl( sin->sin_addr.s_addr ); // s_addr is in network byte order
        
        address16Chars[12] = (unsigned char)((addr >> 24)&0xFF);
        address16Chars[13] = (unsigned char)((addr >> 16)&0xFF);
        address16Chars[14] = (unsigned char)((addr >> 8)&0xFF);
        address16Chars[15] = (unsigned char)(addr&0xFF);
        
        *scopeZoneIndex = 0;
        
        *isIpV6Address = false;
    
    }else if( ai->ai_addr->sa_family == AF_INET6 ){
    
        struct sockaddr_in6 *sin6 = (struct sockaddr_in6*)ai->ai_addr;
        
        std::memcpy( address16Chars, sin6->sin6_addr.__u6_addr.__u6_addr8, 16 );
        
        *scopeZoneIndex = sin6->sin6_scope_id;
        
        *isIpV6Address = true;

    }else{
        std::memset( address16Chars, 0, 16 );        
    }
    
    freeaddrinfo(ai);
}


void GetHostAdresses( std::vector<IpEndpointName> *endpointNames, const char *name )
{
    struct addrinfo *ai;
    struct addrinfo hints;
    std::memset( &hints, 0, sizeof(hints) );
    hints.ai_family = PF_UNSPEC;
    hints.ai_flags = AI_ADDRCONFIG;
    int errcode = getaddrinfo( name, NULL, &hints, &ai );
    if( errcode != 0 )
        assert( errcode == 0 );
    
    struct addrinfo *i = ai;
    while( i != NULL ){
        endpointNames->push_back( IpEndpointName() );
        IpEndpointName &endpointName = endpointNames->back();
        
        if( i->ai_addr->sa_family == AF_INET ){
            const struct sockaddr_in *sin = (const struct sockaddr_in*)i->ai_addr;
            
            endpointName.addressType = IpEndpointName::IPV4_ADDRESS_TYPE;
            
            endpointName.SetIpV4Address(
                                         (sin->sin_addr.s_addr == INADDR_ANY)
                                         ? IpEndpointName::ANY_ADDRESS
                                         : ntohl( sin->sin_addr.s_addr ) );
            
            endpointName.scopeZoneIndex = 0;
            
        }else if( i->ai_addr->sa_family == AF_INET6 ){
            const struct sockaddr_in6 *sin6 = (const struct sockaddr_in6*)i->ai_addr;
            
            endpointName.addressType = IpEndpointName::IPV6_ADDRESS_TYPE;
            
            std::memcpy( endpointName.address, sin6->sin6_addr.__u6_addr.__u6_addr8, 16 );
            
            endpointName.scopeZoneIndex = sin6->sin6_scope_id;
        }
        
        i = i->ai_next;
    }
    
    freeaddrinfo(ai);
}

