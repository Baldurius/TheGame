#pragma once

#include <net/PacketSelector.hpp>
#include <Receiver.hpp>

#include <memory>
#include <thread>
#include <list>

class Client;

class Lobby
{
    public:
        Lobby();

        void add( std::unique_ptr< Client > client );
        void messageCallback( std::unique_ptr< PacketSelector::NetEvent > event );

    private:
        Receiver m_receiver;

        std::list< std::unique_ptr< Client > > m_clients;
};
