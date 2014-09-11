#pragma once

#include <net/PacketSelector.hpp>

#include <mutex>
#include <thread>

class Connection;

class Receiver
{
    public:
        Receiver();

        ~Receiver();

        void add( std::shared_ptr< Connection > connection );
        void remove( std::shared_ptr< Connection > connection );

        void netEvent( std::unique_ptr< PacketSelector::NetEvent > event );

        void run();

    private:
        bool m_running;
        PacketSelector m_packetSelector;
        mutable std::mutex m_mutex;
        std::thread m_thread;
};
