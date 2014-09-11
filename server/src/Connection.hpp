#pragma once

#include <net/SocketContainer.hpp>
#include <net/PacketSelector.hpp>
#include <net/TCPSocket.hpp>

#include <memory>

class Receiver;

class Connection
    : public SocketContainer
    , public std::enable_shared_from_this< Connection >
{
    public:
        Connection(
            const std::shared_ptr< Receiver >& receiver,
            std::unique_ptr< TCPSocket > socket );

        void destroy();

        void setCallback(
            std::function< void ( std::unique_ptr< PacketSelector::NetEvent > ) > callback );

        void netEvent( std::unique_ptr< PacketSelector::NetEvent > event );

        // SocketContainer
        virtual TCPSocket* getSocket() override;

    private:
        std::weak_ptr< Receiver > m_receiver;
        std::unique_ptr< TCPSocket > m_socket;
        std::function< void ( std::unique_ptr< PacketSelector::NetEvent > ) > m_callback;
};



inline Connection::Connection(
    const std::shared_ptr< Receiver >& receiver,
    std::unique_ptr< TCPSocket > socket )
    : m_receiver( receiver )
    , m_socket( std::move( socket ) )
{ }

inline void Connection::setCallback(
    std::function< void ( std::unique_ptr< PacketSelector::NetEvent > ) > callback )
{
    m_callback = callback;
}

inline void Connection::netEvent( std::unique_ptr< PacketSelector::NetEvent > event )
{
    if( m_callback )
        m_callback( std::move( event ) );
}

inline TCPSocket* Connection::getSocket()
{
    return m_socket.get();
}
