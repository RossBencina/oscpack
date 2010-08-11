#ifndef INCLUDED_PACKETLISTENER_H
#define INCLUDED_PACKETLISTENER_H


class IpEndpointName;

class PacketListener{
public:
    virtual ~PacketListener() {}
    virtual void PacketReceived( const char *data, int size, 
			const IpEndpointName& remoteEndpoint ) = 0;
};

#endif /* INCLUDED_PACKETLISTENER_H */
