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
#include "IpEndpointName.h"

#include <cassert>
#include <cstdio>

#include "NetworkingUtils.h"



IpEndpointName::IpEndpointName( const char *addressString, int port_ )
    : addressType( IPV6_ADDRESS_TYPE )
    , scopeZoneIndex( 0 )
    , port( port_ )
{
    std::memset( &address, 0, 16 );

    bool isIpV6Address = true;
        
    IpAddressFromString( address, &scopeZoneIndex, &isIpV6Address, addressString );
    addressType = isIpV6Address ? IPV6_ADDRESS_TYPE : IPV4_ADDRESS_TYPE;    
}


void IpEndpointName::AddressAsString( char *s ) const
{
    if( addressType == IPV4_ADDRESS_TYPE )
    {
        if( IpV4Address() == ANY_ADDRESS ){
            std::sprintf( s, "<any>" );
        }else{
            std::sprintf( s, "%d.%d.%d.%d", address[12], address[13], address[14], address[15] );
        }
    }else{ assert( addressType == IPV6_ADDRESS_TYPE );
        
        // FIXME: do we want to detectIPv4-mapped IPv6 address form (::ffff:0:0/96) and display the last four bytes with dots?
        
        std::sprintf( s, "%x:%x:%x:%x:%x:%x:%x:%x%%%ld",
                     (address[0]<<8)|address[1],
                     (address[2]<<8)|address[3],
                     (address[4]<<8)|address[5],
                     (address[6]<<8)|address[7],
                     (address[8]<<8)|address[9],
                     (address[10]<<8)|address[11],
                     (address[12]<<8)|address[13],
                     (address[14]<<8)|address[15], scopeZoneIndex );
    }
}


void IpEndpointName::AddressAndPortAsString( char *s ) const
{
    if( addressType == IPV4_ADDRESS_TYPE )
    {
        if( IpV4Address() == ANY_ADDRESS ){
            if( port == ANY_PORT ){
                std::sprintf( s, "<any>:<any>" );
            }else{
                std::sprintf( s, "<any>:%d", (int)port );            
            }
        }else{
            if( port == ANY_PORT ){
                std::sprintf( s, "%d.%d.%d.%d:<any>", address[12], address[13], address[14], address[15] );
            }else{
                std::sprintf( s, "%d.%d.%d.%d:%d", address[12], address[13], address[14], address[15], (int)port );
            }
        }
    }else{ assert( addressType == IPV6_ADDRESS_TYPE );
        
        // FIXME: do we want to detectIPv4-mapped IPv6 address form (::ffff:0:0/96) and display the last four bytes with dots?
        
        if( port == ANY_PORT ){        
            std::sprintf( s, "[%x:%x:%x:%x:%x:%x:%x:%x%%%ld]:<any>",
                         (address[0]<<8)|address[1],
                         (address[2]<<8)|address[3],
                         (address[4]<<8)|address[5],
                         (address[6]<<8)|address[7],
                         (address[8]<<8)|address[9],
                         (address[10]<<8)|address[11],
                         (address[12]<<8)|address[13],
                         (address[14]<<8)|address[15], scopeZoneIndex );
        }else{
            std::sprintf( s, "[%x:%x:%x:%x:%x:%x:%x:%x%%%ld]:%d",
                         (address[0]<<8)|address[1],
                         (address[2]<<8)|address[3],
                         (address[4]<<8)|address[5],
                         (address[6]<<8)|address[7],
                         (address[8]<<8)|address[9],
                         (address[10]<<8)|address[11],
                         (address[12]<<8)|address[13],
                         (address[14]<<8)|address[15], scopeZoneIndex, (int)port );
        }
    }
}
