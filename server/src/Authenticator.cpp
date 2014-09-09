#include "Authenticator.hpp"

#include <Client.hpp>
#include <net/OPacket.hpp>
#include <net/TCPSocket.hpp>
#include <constants.hpp>
#include <net/IPacket.hpp>

#include <iostream>

Authenticator::Authenticator(
    std::function< void ( std::unique_ptr< Client > ) > callback )
    : m_receiver(
        std::bind( &Authenticator::messageCallback, this, std::placeholders::_1 ) )
    , m_callback( callback )
{ }

void Authenticator::add( std::unique_ptr< TCPSocket > socket )
{
    m_receiver.add( socket.get() );
    socket.release();
}

void Authenticator::messageCallback( std::unique_ptr< PacketSelector::NetEvent > event )
{
    switch( event->getType() )
    {
        case PacketSelector::NetEvent::Type::CONNECT:
        {
            std::cout << "Authentication..." << std::endl;

            OPacket packet;
            packet.write( CMsg::HELLO );
            packet.send( event->getSource()->getSocket() );
        } break;

        case PacketSelector::NetEvent::Type::CLOSED:
        {
            std::cout << "Disconnect in authentication process..." << std::endl;
            delete event->getSource();
        } break;

        case PacketSelector::NetEvent::Type::BROKEN:
        {
            std::cout << "Broken socket in authentication process..." << std::endl;
            delete event->getSource();
        } break;

        case PacketSelector::NetEvent::Type::PACKET:
        {
            const auto& packet = event->getData< IPacket >();
            switch( packet->read< SMsg >() )
            {
                case SMsg::LOGIN:
                {
                    std::string name = packet->read< std::string >();

                    // login successfull
                    std::cout << "Player " << name << " logged in" << std::endl;
                    TCPSocket* socket = static_cast< TCPSocket* >( event->getSource() );
                    m_receiver.remove( socket );

                    m_callback( std::unique_ptr< Client >( new Client(
                        std::unique_ptr< TCPSocket >( socket ),
                        std::move( name ) ) ) );
                } break;

                default:
                    std::cout << "Authenticator was unable to understand client" << std::endl;
            }
        } break;
    }
}
