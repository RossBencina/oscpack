#include "IpEndpointName.h"

#include <stdio.h>

#include "NetworkingUtils.h"


unsigned long IpEndpointName::GetHostByName( const char *s )
{
	return ::GetHostByName(s);
}


void IpEndpointName::AddressAsString( char *s ) const
{
	if( address == ANY_ADDRESS ){
		sprintf( s, "<any>" );
	}else{
		sprintf( s, "%d.%d.%d.%d",
				(int)((address >> 24) & 0xFF),
				(int)((address >> 16) & 0xFF),
				(int)((address >> 8) & 0xFF),
				(int)(address & 0xFF) );
	}
}


void IpEndpointName::AddressAndPortAsString( char *s ) const
{
	if( port == ANY_PORT ){
		if( address == ANY_ADDRESS ){
			sprintf( s, "<any>:<any>" );
		}else{
			sprintf( s, "%d.%d.%d.%d:<any>",
				(int)((address >> 24) & 0xFF),
				(int)((address >> 16) & 0xFF),
				(int)((address >> 8) & 0xFF),
				(int)(address & 0xFF) );
		}
	}else{
		if( address == ANY_ADDRESS ){
			sprintf( s, "<any>:%d", port );
		}else{
			sprintf( s, "%d.%d.%d.%d:%d",
				(int)((address >> 24) & 0xFF),
				(int)((address >> 16) & 0xFF),
				(int)((address >> 8) & 0xFF),
				(int)(address & 0xFF),
				(int)port );
		}
	}	
}
