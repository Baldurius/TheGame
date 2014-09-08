#pragma once

#include <net/PacketSelector.hpp>

#include <mutex>
#include <thread>

class Receiver
{
    public:
        Receiver(
            std::unique_ptr< PacketSelector > selector,
            std::function< void ( std::unique_ptr< PacketSelector::NetEvent > ) > callback );

        ~Receiver();

        void run();

    private:
        bool m_running;
        std::unique_ptr< PacketSelector > m_selector;
        std::function< void ( std::unique_ptr< PacketSelector::NetEvent > ) > m_callback;
        mutable std::mutex m_mutex;
        std::thread m_thread;
};
