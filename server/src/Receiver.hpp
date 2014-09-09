#pragma once

#include <net/PacketSelector.hpp>

#include <mutex>
#include <thread>

class SocketContainer;

class Receiver
{
    public:
        Receiver(
            std::function< void ( std::unique_ptr< PacketSelector::NetEvent > ) > callback );

        ~Receiver();

        void add( SocketContainer* container );
        void remove( SocketContainer* container );

        void run();

    private:
        bool m_running;
        PacketSelector m_packetSelector;
        mutable std::mutex m_mutex;
        std::thread m_thread;
};
