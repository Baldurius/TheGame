#pragma once

#include <net/SocketContainer.hpp>

#include <memory>

class Connection;

class Client
{
    public:
        Client(
            std::shared_ptr< Connection > connection,
            std::string name );

        const std::shared_ptr< Connection >& getConnection() const;
        const std::string& getName() const;

    private:
        std::shared_ptr< Connection > m_connection;
        std::string m_name;
};



inline const std::shared_ptr< Connection >& Client::getConnection() const
{
    return m_connection;
}

inline const std::string& Client::getName() const
{
    return m_name;
}
