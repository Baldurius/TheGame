#include "Client.hpp"

#include <net/TCPSocket.hpp>

Client::Client( std::unique_ptr< TCPSocket > socket )
    : m_socket( std::move( socket ) )
{ }
