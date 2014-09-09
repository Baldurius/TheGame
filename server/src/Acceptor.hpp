#pragma once

#include <mutex>
#include <cstdint>
#include <memory>
#include <thread>

class TCPSocket;

class Acceptor
{
    public:
        Acceptor(
            uint16_t port,
            std::function< void ( std::unique_ptr< TCPSocket > ) > callback );

        ~Acceptor();

        void run();

    private:
        bool m_running;
        uint16_t m_port;
        std::function< void ( std::unique_ptr< TCPSocket > ) > m_callback;
        mutable std::mutex m_mutex;
        std::thread m_thread;
};
