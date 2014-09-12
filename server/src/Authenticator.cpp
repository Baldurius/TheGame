#include "Authenticator.hpp"

#include <Client.hpp>
#include <net/OPacket.hpp>
#include <net/TCPSocket.hpp>
#include <constants.hpp>
#include <net/IPacket.hpp>
#include <Connection.hpp>

#include <iostream>

class Authenticator::LoginHandler
{
    public:
        LoginHandler(
            std::shared_ptr< Authenticator > authenticator,
            std::shared_ptr< Connection > connection )
            : m_authenticator( std::move( authenticator ) )
            , m_connection( std::move( connection ) )
        {
            m_connection->setCallback(
                std::bind( &LoginHandler::netEvent, this, std::placeholders::_1 ) );
        }

        void netEvent( std::unique_ptr< PacketSelector::NetEvent > event )
        {
            switch( event->getType() )
            {
                case PacketSelector::NetEvent::Type::CONNECT:
                {
                    std::cout << "Authentication..." << std::endl;

                    OPacket packet;
                    packet.write( ClientMessage::HELLO );
                    packet.write( std::string( "Bitte logge dich ein!" ) );
                    packet.send( m_connection->getSocket() );
                } break;

                case PacketSelector::NetEvent::Type::CLOSED:
                case PacketSelector::NetEvent::Type::BROKEN:
                {
                    std::cout << "Disconnect in authentication process..." << std::endl;
                    delete this;
                } break;

                case PacketSelector::NetEvent::Type::PACKET:
                {
                    const auto& packet = event->getData< IPacket >();
                    switch( packet->read< ServerMessage >() )
                    {
                        case ServerMessage::LOGIN:
                        {
                            std::string name = packet->read< std::string >();

                            // login successfull
                            std::cout << "Player " << name << " logged in" << std::endl;

                            m_authenticator->m_callback(
                                std::unique_ptr< Client >( new Client(
                                    std::move( m_connection ),
                                    std::move( name ) ) ) );

                            delete this;
                        } break;

                        default:
                            std::cout << "Authenticator was unable to understand client" << std::endl;
                    }
                } break;
            }
        }

    private:
        std::shared_ptr< Authenticator > m_authenticator;
        std::shared_ptr< Connection > m_connection;
};

Authenticator::Authenticator(
    std::function< void ( std::unique_ptr< Client > ) > callback )
    : m_callback( callback )
{ }

void Authenticator::add( std::shared_ptr< Connection > connection )
{
    new LoginHandler(
        shared_from_this(),
        std::move( connection ) );
}
