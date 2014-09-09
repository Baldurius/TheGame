#pragma once

#include <net/PacketSelector.hpp>
#include <Receiver.hpp>

#include <thread>

class Client;

class Authenticator
{
    public:
        Authenticator(
            std::function< void ( std::unique_ptr< Client > ) > callback );

        void add( std::unique_ptr< TCPSocket > socket );
        void messageCallback( std::unique_ptr< PacketSelector::NetEvent > event );

    private:
        Receiver m_receiver;
        std::function< void ( std::unique_ptr< Client > ) > m_callback;
};
