#pragma once

#include <net/PacketSelector.hpp>
#include <Receiver.hpp>

#include <thread>

class Client;

class Authenticator
    : public std::enable_shared_from_this< Authenticator >
{
    public:
        Authenticator(
            std::function< void ( std::unique_ptr< Client > ) > callback );

        void add( std::shared_ptr< Connection > connection );

    private:
        class LoginHandler;

        std::function< void ( std::unique_ptr< Client > ) > m_callback;
};
