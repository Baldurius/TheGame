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
        void add( std::unique_ptr< Client > client );

    private:
        std::list< std::unique_ptr< Client > > m_clients;
};
