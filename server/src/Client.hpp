#pragma once

#include <net/SocketContainer.hpp>

#include <memory>

class Client
    : public SocketContainer
{
    public:
        Client( std::unique_ptr< TCPSocket > socket );

        // SocketContainer
        virtual TCPSocket* getSocket() override; 

    private:
        std::unique_ptr< TCPSocket > m_socket;
};



inline TCPSocket* Client::getSocket()
{
    return m_socket.get();
}
