#include "Lobby.hpp"

#include <Client.hpp>
#include <net/IPacket.hpp>
#include <net/TCPSocket.hpp>
#include <constants.hpp>
#include <game/Game.hpp>

#include <iostream>

Lobby::Lobby()
    : m_receiver(
        std::bind( &Lobby::messageCallback, this, std::placeholders::_1 ) )
{ }

void Lobby::add( std::unique_ptr< Client > client )
{
    m_receiver.add( client.get() );
    m_clients.push_back( std::move( client ) );
}

void Lobby::messageCallback( std::unique_ptr< PacketSelector::NetEvent > event )
{
    Client* client = static_cast< Client* >( event->getSource() );

    switch( event->getType() )
    {
        case PacketSelector::NetEvent::Type::CONNECT:
        {
            std::cout << client->getName() << " joined lobby!" << std::endl;

            if( m_clients.size() >= 2 )
            {
                auto player1 = std::move( m_clients.front() );
                m_clients.pop_front();
                auto player2 = std::move( m_clients.front() );
                m_clients.pop_front();

                m_receiver.remove( player1.get() );
                m_receiver.remove( player2.get() );

                new Game(
                    std::move( player1 ),
                    std::move( player2 ),
                    std::bind( &Lobby::add, this, std::placeholders::_1 ) );
            }
        } break;

        case PacketSelector::NetEvent::Type::CLOSED:
        {
            std::cout << client->getName() << " disconnected" << std::endl;
            delete client;
        } break;

        case PacketSelector::NetEvent::Type::BROKEN:
        {
            std::cout << client->getName() << " timed out" << std::endl;
            delete client;
        } break;

        case PacketSelector::NetEvent::Type::PACKET:
        {
            const auto& packet = event->getData< IPacket >();
            switch( packet->read< SMsg >() )
            {
                default:
                    std::cout << "Lobby was unable to understand client" << std::endl;
            }
        } break;
    }
}
