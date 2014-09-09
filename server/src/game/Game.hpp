#pragma once

#include <net/PacketSelector.hpp>
#include <Receiver.hpp>

#include <mutex>
#include <condition_variable>
#include <thread>
#include <memory>

class Client;

class Game
{
    public:
        Game(
            std::unique_ptr< Client > player1,
            std::unique_ptr< Client > player2,
            std::function< void ( std::unique_ptr< Client > ) > leaveCallback );

        ~Game();

        void messageCallback( std::unique_ptr< PacketSelector::NetEvent > event ); 
        void run();

    private:
        Receiver m_receiver;

        std::unique_ptr< Client > m_player1;
        std::unique_ptr< Client > m_player2;

        std::function< void ( std::unique_ptr< Client > ) > m_leaveCallback;

        bool m_running;
        std::condition_variable m_condition;
        mutable std::mutex m_mutex;
        std::thread m_thread;
};
