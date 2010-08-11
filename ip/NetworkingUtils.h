#ifndef INCLUDED_NETWORKINGUTILS_H
#define INCLUDED_NETWORKINGUTILS_H

void InitializeNetworking();
void TerminateNetworking();

#define PACK_IPV4_ADDRESS( a, b, c, d )\
    ( ((d) << 24) | ((c) << 16) | ((b) << 8) | (a) )
    
unsigned long GetHostByName( const char *name );

#endif /* INCLUDED_NETWORKINGUTILS_H */
