#ifndef INCLUDED_NETWORKINGUTILS_H
#define INCLUDED_NETWORKINGUTILS_H


// in general NetworkInitializer is only used internally, but if you're 
// application creates multiple sockets from different threads at runtime you
// should instantiate one of these in main just to make sure the networking
// layer is initialized.
class NetworkInitializer{
public:
    NetworkInitializer();
    ~NetworkInitializer();
};


// return ip address of host name in host byte order
unsigned long GetHostByName( const char *name );


#endif /* INCLUDED_NETWORKINGUTILS_H */
