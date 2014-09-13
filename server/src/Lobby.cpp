#include "Lobby.hpp"

#include <Client.hpp>
#include <net/IPacket.hpp>
#include <net/TCPSocket.hpp>
#include <constants.hpp>
#include <game/Game.hpp>
#include <Connection.hpp>

#include <iostream>

class Lobby::ClientWrapper
    : public std::enable_shared_from_this< ClientWrapper >
{
    public:
        ClientWrapper(
            std::shared_ptr< Lobby > lobby,
            std::unique_ptr< Client > client )
            : m_lobby( std::move( lobby ) )
            , m_client( std::move( client ) )
        {
            m_client->getConnection()->setCallback(
                std::bind( &ClientWrapper::netEvent, this, std::placeholders::_1 ) );
        }

        void netEvent( std::unique_ptr< PacketSelector::NetEvent > event )
        {
            std::unique_lock< std::mutex > lock( m_lobby->m_mutex );
            m_lobby->m_events.push_back( std::make_tuple(
                shared_from_this(),
                std::move( event ) ) );
            m_lobby->m_condition.notify_all();
        }

        const std::unique_ptr< Client >& getClient() const
        {
            return m_client;
        }

        std::unique_ptr< Client > detachClient()
        {
            m_client->getConnection()->setCallback();
            return std::move( m_client );
        }

    private:
        std::shared_ptr< Lobby > m_lobby;
        std::unique_ptr< Client > m_client;
};

Lobby::Lobby()
    : m_running( true )
    , m_thread( std::bind( &Lobby::run, this ) )
{
}

Lobby::~Lobby()
{
    std::unique_lock< std::mutex > lock( m_mutex );
    m_running = false;
    m_condition.notify_all();
    lock.unlock();
}

void Lobby::add( std::unique_ptr< Client > client )
{
    std::unique_lock< std::mutex > lock( m_mutex );

    auto connection = client->getConnection();
    auto clientWrapper = std::make_shared< ClientWrapper >(
        shared_from_this(), std::move( client ) );

    m_clients.insert( clientWrapper );

    m_events.push_back( std::make_tuple(
        std::move( clientWrapper ),
        std::unique_ptr< PacketSelector::NetEvent >( new PacketSelector::NetEvent(
            PacketSelector::NetEvent::Type::CONNECT,
            connection ) ) ) );

    m_condition.notify_all();
}

void Lobby::run()
{
    std::unique_lock< std::mutex > lock( m_mutex );
    while( m_running )
    {
        while( !m_events.empty() )
        {
            auto eventTup = std::move( m_events.front() );
            m_events.pop_front();
            lock.unlock();

            auto& event = std::get< 1 >( eventTup );
            auto& wrapper = std::get< 0 >( eventTup );

            switch( event->getType() )
            {
                case PacketSelector::NetEvent::Type::CONNECT:
                {
                    std::cout << wrapper->getClient()->getName() << " joined the lobby!" << std::endl; 

                    if( m_clients.size() >= 2 )
                    {
                        auto iterator = m_clients.begin();
                        auto player1 = (*iterator)->detachClient();

                        iterator = m_clients.erase( iterator );
                        auto player2 = (*iterator)->detachClient();

                        Game::create(
                            std::move( player1 ),
                            std::move( player2 ) );
                    }
                } break;

                case PacketSelector::NetEvent::Type::BROKEN:
                case PacketSelector::NetEvent::Type::CLOSED:
                {
                    std::cout << wrapper->getClient()->getName() << " left lobby!" << std::endl; 
                    lock.lock();
                    m_clients.erase( std::move( wrapper ) );
                    lock.unlock();
                } break;

                case PacketSelector::NetEvent::Type::PACKET:
                {
                } break;
            }

            lock.lock();
        }
        m_condition.wait( lock );
    }
}
