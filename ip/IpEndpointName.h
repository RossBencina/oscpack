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
#ifndef INCLUDED_OSCPACK_IPENDPOINTNAME_H
#define INCLUDED_OSCPACK_IPENDPOINTNAME_H

#include <cstring> // memset, memcmp

// represents and ip address and port

class IpEndpointName{    
public:
    enum AddressType { IPV4_ADDRESS_TYPE, IPV6_ADDRESS_TYPE };
    
    static const unsigned long ANY_ADDRESS = 0xFFFFFFFF;
    static const int ANY_PORT = -1;

    // Always represent ANY_ADDRESS as an IPv4 address with our ANY_ADDRESS value
    // This allows the socket code to detect and map an ipv4 any address to an
    // ipv6 any address when desired
    IpEndpointName()
        : addressType( IPV4_ADDRESS_TYPE )
        , scopeZoneIndex( 0 )
        , port( ANY_PORT )
    {
        SetIpV4Address( ANY_ADDRESS );
    }
    
    IpEndpointName( int port_ )
        : addressType( IPV4_ADDRESS_TYPE )
        , scopeZoneIndex( 0 )
        , port( port_ )
    {
        SetIpV4Address( ANY_ADDRESS );
    }
    
    IpEndpointName( unsigned long ipV4Address_, int port_ )
        : addressType( IPV4_ADDRESS_TYPE )
        , scopeZoneIndex( 0 )
        , port( port_ )
    {
        SetIpV4Address( ipV4Address_ );
    }
    
    // due to the way IPv6 works, it isn't really appropriate to
    // use this ctor with domain names.
    // safe for localhost and numeric ip address strings only
    IpEndpointName( const char *addressString, int port=ANY_PORT );
    
    // construct with "dotted" ipv4 8 bit address components as params. eg. 127,0,0,1
    IpEndpointName( int addressA, int addressB, int addressC, int addressD, int port_=ANY_PORT )
        : addressType( IPV4_ADDRESS_TYPE )
        , scopeZoneIndex( 0 )
        , port( port_ )
    {
        // IMPORTANT: IPv4 adresses are ALWAYS stored in IPv4-mapped IPv6 address form (::ffff:0:0/96)
        
        std::memset( &address, 0, 10 );
        
        address[10] = 0xFF;
        address[11] = 0xFF;
        
        address[12] = (unsigned char)addressA;
        address[13] = (unsigned char)addressB;
        address[14] = (unsigned char)addressC;
        address[15] = (unsigned char)addressD;
    }
    
    
    // construct with "coloned" ipv6 16 bit address components as params. eg. 0xfe80,0,0,0,0x3e07,0x54ff,0xfe03,0x5a1a
    IpEndpointName( int addressA, int addressB, int addressC, int addressD,
                    int addressE, int addressF, int addressG, int addressH, int port_=ANY_PORT )
        : addressType( IPV6_ADDRESS_TYPE )
        , scopeZoneIndex( 0 )
        , port( port_ )
    {
        address[0] = (unsigned char)((addressA >> 8) & 0xFFUL);
        address[1] = (unsigned char)(addressA & 0xFFUL);
        
        address[2] = (unsigned char)((addressB >> 8) & 0xFFUL);
        address[3] = (unsigned char)(addressB & 0xFFUL);
        
        address[4] = (unsigned char)((addressC >> 8) & 0xFFUL);
        address[5] = (unsigned char)(addressC & 0xFFUL);
        
        address[6] = (unsigned char)((addressD >> 8) & 0xFFUL);
        address[7] = (unsigned char)(addressD & 0xFFUL);
        
        address[8] = (unsigned char)((addressE >> 8) & 0xFFUL);
        address[9] = (unsigned char)(addressE & 0xFFUL);
        
        address[10] = (unsigned char)((addressF >> 8) & 0xFFUL);
        address[11] = (unsigned char)(addressF & 0xFFUL);
        
        address[12] = (unsigned char)((addressG >> 8) & 0xFFUL);
        address[13] = (unsigned char)(addressG & 0xFFUL);
        
        address[14] = (unsigned char)((addressH >> 8) & 0xFFUL);
        address[15] = (unsigned char)(addressH & 0xFFUL);
    }
    
	// address and port are maintained in host byte order here
    char addressType; // AddressType
    
    // IMPORTANT: IPv4 adresses are ALWAYS stored in IPv4-mapped IPv6 address form (::ffff:0:0/96)
    unsigned char address[16];
    
    unsigned long scopeZoneIndex;
    
    int port;

    unsigned long IpV4Address() const
    {
        return (((unsigned long)address[12]) << 24)
                | (((unsigned long)address[13]) << 16)
                | (((unsigned long)address[14]) << 8)
                | ((unsigned long)address[15]);
    }
    
    void SetIpV4Address( unsigned long ipV4Address_ )
    {
        // IMPORTANT: IPv4 adresses are ALWAYS stored in IPv4-mapped IPv6 address form (::ffff:0:0/96)
        
        std::memset( &address, 0, 10 );
        
        address[10] = 0xFF;
        address[11] = 0xFF;
        
        address[12] = (unsigned char)((ipV4Address_ >> 24) & 0xFFUL);
        address[13] = (unsigned char)((ipV4Address_ >> 16) & 0xFFUL);
        address[14] = (unsigned char)((ipV4Address_ >> 8) & 0xFFUL);
        address[15] = (unsigned char)(ipV4Address_ & 0xFFUL);        
    }
    
    //bool IsMulticastAddress() const { return ((address >> 24) & 0xFF) >= 224 && ((address >> 24) & 0xFF) <= 239; }

	enum { ADDRESS_STRING_LENGTH=(4*8)+7+1 +20 }; // FIXME this is wrong now, the +20 is intended to account for the zone id
	void AddressAsString( char *s ) const;

	enum { ADDRESS_AND_PORT_STRING_LENGTH=(4*8)+7+1+5 +20 }; // FIXME this is wrong now, the +20 is intended to account for the zone id
	void AddressAndPortAsString( char *s ) const;
};

inline bool operator==( const IpEndpointName& lhs, const IpEndpointName& rhs )
{	
	return (lhs.addressType == rhs.addressType && lhs.port == rhs.port && std::memcmp(lhs.address,rhs.address,16) == 0 );
}

inline bool operator!=( const IpEndpointName& lhs, const IpEndpointName& rhs )
{
	return !(lhs == rhs);
}

#endif /* INCLUDED_OSCPACK_IPENDPOINTNAME_H */
