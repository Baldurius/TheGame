#pragma once

#include <net/PacketSelector.hpp>
#include <Receiver.hpp>

#include <mutex>
#include <condition_variable>
#include <thread>
#include <memory>

class Client;
class Event;

class Game
    : public std::enable_shared_from_this< Game >
{
    public:
        static std::shared_ptr< Game > create(
            std::unique_ptr< Client > player1,
            std::unique_ptr< Client > player2 );

        void addEvent( std::shared_ptr< Event > event );

        void run();

    private:
        class Player;

        static std::list< std::shared_ptr< Game > > s_activeGames;

        Game(
            std::unique_ptr< Client > player1,
            std::unique_ptr< Client > player2 );

        std::shared_ptr< Player > m_player1;
        std::shared_ptr< Player > m_player2;

        std::list< std::tuple<
            std::shared_ptr< Player >,
            std::unique_ptr< PacketSelector::NetEvent > > > m_netEvents;

        std::vector< std::shared_ptr< Event > > m_gameEvents;

        bool m_running;
        std::condition_variable m_condition;
        mutable std::mutex m_mutex;
        std::thread m_thread;
};

class Game::Player
    : public std::enable_shared_from_this< Player >
{
    public:
        Player(
            std::shared_ptr< Game > game,
            std::unique_ptr< Client > client );

        void netEvent(
            std::unique_ptr< PacketSelector::NetEvent > event );

        const std::unique_ptr< Client >& getClient() const;

    private:
        std::shared_ptr< Game > m_game;
        std::unique_ptr< Client > m_client;
};
