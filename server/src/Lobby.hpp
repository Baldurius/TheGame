#pragma once

#include <net/PacketSelector.hpp>
#include <Receiver.hpp>

#include <memory>
#include <thread>
#include <list>
#include <mutex>
#include <condition_variable>
#include <set>

class Client;

class Lobby
    : public std::enable_shared_from_this< Lobby >
{
    class ClientWrapper;

    public:
        Lobby();

        ~Lobby();

        void add( std::unique_ptr< Client > client );

        void netEvent(
            const std::shared_ptr< ClientWrapper >& wrapper,
            std::unique_ptr< PacketSelector::NetEvent > event );

        void run();

    private:
        std::set< std::shared_ptr< ClientWrapper > > m_clients;
        std::list< std::tuple<
            std::shared_ptr< ClientWrapper >,
            std::unique_ptr< PacketSelector::NetEvent > > > m_events;

        bool m_running;
        std::condition_variable m_condition;
        mutable std::mutex m_mutex;
        std::thread m_thread;
};
