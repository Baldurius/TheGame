#pragma once

#include <net/PacketSelector.hpp>

#include <thread>

class Client;

class Authenticator
{
    public:
        Authenticator(
            std::function< void ( std::unique_ptr< Client > ) > callback );

        ~Authenticator();

        void add( std::unique_ptr< TCPSocket > socket );

        void messageCallback( std::unique_ptr< PacketSelector::NetEvent > event );

        void run();

    private:
        bool m_running;
        PacketSelector m_packetSelector;
        std::function< void ( std::unique_ptr< Client > ) > m_callback;
        mutable std::mutex m_mutex;
        std::thread m_thread;
};
