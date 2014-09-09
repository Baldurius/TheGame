#include "Game.hpp"

#include <Client.hpp>
#include <net/IPacket.hpp>
#include <net/TCPSocket.hpp>
#include <constants.hpp>

#include <iostream>

Game::Game(
    std::unique_ptr< Client > player1,
    std::unique_ptr< Client > player2,
    std::function< void ( std::unique_ptr< Client > ) > leaveCallback )
    : m_receiver(
        std::bind( &Game::messageCallback, this, std::placeholders::_1 ) )
    , m_player1( std::move( player1 ) )
    , m_player2( std::move( player2 ) )
    , m_leaveCallback( std::move( leaveCallback ) )
    , m_running( true )
    , m_thread(
        std::bind( &Game::run, this ) )
{
    m_receiver.add( m_player1.get() );
    m_receiver.add( m_player2.get() );
}

Game::~Game()
{
    std::cout << "del Game1" << std::endl;
    std::unique_lock< std::mutex > lock( m_mutex );
    m_running = false;
    m_condition.notify_all();
    lock.unlock();

    m_thread.join();
    std::cout << "del Game2" << std::endl;
}

void Game::messageCallback( std::unique_ptr< PacketSelector::NetEvent > event )
{
    Client* client = static_cast< Client* >( event->getSource() );

    switch( event->getType() )
    {
        case PacketSelector::NetEvent::Type::CONNECT:
        {
            std::cout << client->getName() << " joined the game!" << std::endl;
        } break;

        case PacketSelector::NetEvent::Type::CLOSED:
        case PacketSelector::NetEvent::Type::BROKEN:
        {
            std::cout << client->getName() << " left the game!" << std::endl;

            if( client == m_player1.get() )
            {
                m_receiver.remove( m_player2.get() );
                m_leaveCallback( std::move( m_player2 ) );
            }
            else
            { 
                m_receiver.remove( m_player1.get() );
                m_leaveCallback( std::move( m_player1 ) );
            }

            delete this;
        } break;

        case PacketSelector::NetEvent::Type::PACKET:
        {
            const auto& packet = event->getData< IPacket >();
            switch( packet->read< SMsg >() )
            {
                default:
                    std::cout << "Game was unable to understand client" << std::endl;
            }
        } break;

    }
}

void Game::run()
{
    std::unique_lock< std::mutex > lock( m_mutex );
    while( m_running )
    {
        m_condition.wait( lock );
    }
}
