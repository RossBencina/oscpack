#ifndef INCLUDED_IPENDPOINTNAME_H
#define INCLUDED_IPENDPOINTNAME_H


class IpEndpointName{
	static unsigned long GetHostByName( const char *s );
public:
    static const unsigned long ANY_ADDRESS = 0xFFFFFFFF;
    static const int ANY_PORT = -1;

    IpEndpointName()
		: address( ANY_ADDRESS ), port( ANY_PORT ) {}
    IpEndpointName( int port_ ) 
		: address( ANY_ADDRESS ), port( port_ ) {}
    IpEndpointName( unsigned long ipAddress_, int port_ ) 
		: address( ipAddress_ ), port( port_ ) {}
    IpEndpointName( const char *addressName, int port_=ANY_PORT )
		: address( GetHostByName( addressName ) )
		, port( port_ ) {}
    IpEndpointName( int addressA, int addressB, int addressC, int addressD, int port_=ANY_PORT )
		: address( ( (addressA << 24) | (addressB << 16) | (addressC << 8) | addressD ) )
		, port( port_ ) {}

	// address and port are maintained in host byte order here
    unsigned long address;
    int port;

	enum { ADDRESS_STRING_LENGTH=17 };
	void AddressAsString( char *s ) const;

	enum { ADDRESS_AND_PORT_STRING_LENGTH=23};
	void AddressAndPortAsString( char *s ) const;
};

inline bool operator==( const IpEndpointName& lhs, const IpEndpointName& rhs )
{	
	return (lhs.address == rhs.address && lhs.port == rhs.port );
}

inline bool operator!=( const IpEndpointName& lhs, const IpEndpointName& rhs )
{
	return !(lhs == rhs);
}

#endif /* INCLUDED_IPENDPOINTNAME_H */
