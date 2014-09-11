#pragma once

#include <mutex>
#include <cstdint>
#include <memory>
#include <thread>

class TCPSocket;
class Receiver;
class Connection;

class Acceptor
{
    public:
        Acceptor(
            uint16_t port,
            std::shared_ptr< Receiver > receiver,
            std::function< void ( std::shared_ptr< Connection > ) > callback );

        ~Acceptor();

        void run();

    private:
        bool m_running;
        uint16_t m_port;
        std::shared_ptr< Receiver > m_receiver;
        std::function< void ( std::shared_ptr< Connection > ) > m_callback;
        mutable std::mutex m_mutex;
        std::thread m_thread;
};
