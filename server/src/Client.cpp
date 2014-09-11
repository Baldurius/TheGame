#include "Client.hpp"

#include <net/TCPSocket.hpp>

Client::Client(
    std::shared_ptr< Connection > connection,
    std::string name )
    : m_connection( std::move( connection ) )
    , m_name( std::move( name ) )
{ }
