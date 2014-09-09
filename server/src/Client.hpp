#pragma once

#include <net/SocketContainer.hpp>

#include <memory>

class Client
    : public SocketContainer
{
    public:
        Client(
            std::unique_ptr< TCPSocket > socket,
            std::string name );

        // SocketContainer
        virtual TCPSocket* getSocket() override; 

        const std::string& getName() const;

    private:
        std::unique_ptr< TCPSocket > m_socket;
        std::string m_name;
};



inline TCPSocket* Client::getSocket()
{
    return m_socket.get();
}

inline const std::string& Client::getName() const
{
    return m_name;
}
