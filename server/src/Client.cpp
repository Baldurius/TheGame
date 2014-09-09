#include "Client.hpp"

#include <net/TCPSocket.hpp>

Client::Client(
    std::unique_ptr< TCPSocket > socket,
    std::string name )
    : m_socket( std::move( socket ) )
    , m_name( std::move( name ) )
{ }
