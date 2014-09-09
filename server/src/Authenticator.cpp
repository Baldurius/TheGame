#include "Authenticator.hpp"

#include <Client.hpp>
#include <net/OPacket.hpp>
#include <net/TCPSocket.hpp>
#include <constants.hpp>
#include <net/IPacket.hpp>

#include <iostream>

Authenticator::Authenticator(
    std::function< void ( std::unique_ptr< Client > ) > callback )
    : m_running( true )
    , m_callback( callback )
    , m_packetSelector(
        std::bind( &Authenticator::messageCallback, this, std::placeholders::_1 ) )
    , m_thread(
        std::bind( &Authenticator::run, this ) )
{ }

Authenticator::~Authenticator()
{
    std::unique_lock< std::mutex > lock( m_mutex );
    m_running = false;
    m_packetSelector.notify();
    lock.unlock();

    m_thread.join();
}

void Authenticator::add( std::unique_ptr< TCPSocket > socket )
{
    Client* client = new Client( std::move( socket ) );
    m_packetSelector.add( client );
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
                    std::cout << "Login" << std::endl;
                } break;

                default:
                {
                    std::cout << "Authenticator does not understand the client" << std::endl;
                }
            }
        } break;
    }
}

void Authenticator::run()
{
    std::unique_lock< std::mutex > lock( m_mutex );
    while( m_running )
    {
        lock.unlock();
        m_packetSelector.wait();
        lock.lock();
    }
}
