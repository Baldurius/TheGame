#include "Game.hpp"

#include <Client.hpp>
#include <net/IPacket.hpp>
#include <net/TCPSocket.hpp>
#include <constants.hpp>
#include <Connection.hpp>
#include <net/OPacket.hpp>
#include <game/Event.hpp>
#include <game/CallbackEvent.hpp>
#include <game/entities/Worker.hpp>

#include <iostream>
#include <algorithm>

std::list< std::shared_ptr< Game > > Game::s_activeGames;

std::shared_ptr< Game > Game::create(
    std::unique_ptr< Client > player1,
    std::unique_ptr< Client > player2 )
{
    std::shared_ptr< Game > game( new Game(
        std::move( player1 ), std::move( player2 ) ) );

    s_activeGames.push_back( game );

    return std::move( game );
}

Game::Game(
    std::unique_ptr< Client > player1,
    std::unique_ptr< Client > player2 )
    : m_player1( new Player(
        this,
        std::move( player1 ) ) )
    , m_player2( new Player(
        this,
        std::move( player2 ) ) )
    , m_running( true )
    , m_thread( std::bind( &Game::run, this ) )
{ }

void Game::addEvent( std::shared_ptr< Event > event )
{
    m_gameEvents.push_back( std::move( event ) );
    std::push_heap( m_gameEvents.begin(), m_gameEvents.end(), Event::compare );
}

void Game::run()
{
    // initialize game
    std::cout << "Starting game..." << std::endl;

    auto entity1 = entities::Worker::Creator::create(
        shared_from_this(), 0.0f, 0.0f );

    { // inform players about started game
        OPacket packet;
        packet.write( ClientMessage::GAME_START );
        //packet.send( m_player1->getClient()->getConnection()->getSocket() );
        //packet.send( m_player2->getClient()->getConnection()->getSocket() );
    }

    std::unique_lock< std::mutex > lock( m_mutex );
    while( m_running )
    {
        auto currentTime = std::chrono::steady_clock::now();

        // check for net events
        while( !m_netEvents.empty() )
        {
            auto tuple = std::move( m_netEvents.front() );
            m_netEvents.pop_front();
        }

        // check game events
        if( !m_gameEvents.empty() )
        {
            auto& event = m_gameEvents.front();

            while( !event->isValid() || event->getTimePoint() <= currentTime )
            {
                if( event->isValid() )
                {
                    // event is ready to go!
                    event->execute();
                }

                std::pop_heap( m_gameEvents.begin(), m_gameEvents.end(), Event::compare );
                m_gameEvents.pop_back();

                if( m_gameEvents.empty() )
                    break;
                else
                    event = m_gameEvents.front();
            }
        }

        if( !m_gameEvents.empty() )
            m_condition.wait_until( lock, m_gameEvents.front()->getTimePoint() );
        else
            m_condition.wait( lock );
    }
}



Game::Player::Player(
    Game* game,
    std::unique_ptr< Client > client )
    : m_game( game )
    , m_client( std::move( client ) )
{
    if( m_client )
        m_client->getConnection()->setCallback(
            std::bind( &Player::netEvent, this, std::placeholders::_1 ) );
}

void Game::Player::netEvent( std::unique_ptr< PacketSelector::NetEvent > event )
{
    // add event to game queue to push it directly to the game-thread
    std::unique_lock< std::mutex > lock( m_game->m_mutex );
    m_game->m_netEvents.push_back( std::make_tuple(
        shared_from_this(),
        std::move( event ) ) );
    m_game->m_condition.notify_all();
}

const std::unique_ptr< Client >& Game::Player::getClient() const
{
    return m_client;
}
