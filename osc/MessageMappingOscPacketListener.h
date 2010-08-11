#ifndef INCLUDED_MESSAGEMAPPINGOSCPACKETLISTENER_H
#define INCLUDED_MESSAGEMAPPINGOSCPACKETLISTENER_H

#include <string.h>
#include <map>

#include "OscPacketListener.h"



namespace osc{

template< class T >
class MessageMappingOscPacketListener : public OscPacketListener{
public:
    typedef void (T::*function_type)(const osc::ReceivedMessage&, const IpEndpointName&);

protected:
    void RegisterMessageFunction( const char *addressPattern, function_type f )
    {
        functions_.insert( std::make_pair( addressPattern, f ) );
    }

    virtual void ProcessMessage( const osc::ReceivedMessage& m,
		const IpEndpointName& remoteEndpoint )
    {
        typename function_map_type::iterator i = functions_.find( m.AddressPattern() );
        if( i != functions_.end() )
            (dynamic_cast<T*>(this)->*(i->second))( m, remoteEndpoint );
    }
    
private:
    struct cstr_compare{
        bool operator()( const char *lhs, const char *rhs ) const
            { return strcmp( lhs, rhs ) < 0; }
    };

    typedef std::map<const char*, function_type, cstr_compare> function_map_type;
    function_map_type functions_;
};

} // namespace osc

#endif /* INCLUDED_MESSAGEMAPPINGOSCPACKETLISTENER_H */