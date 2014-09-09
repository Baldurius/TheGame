#pragma once

#include "SocketContainer.hpp"

#include <string>

#ifdef WIN32
    #include <winsock2.h>
#endif

class TCPSocket : public SocketContainer
{
    public:
        enum class Error
        {
            CREATE,
            CLOSED,
            BROKEN,
            CONNECT
        };

        TCPSocket( const std::string& address, int port );
        ~TCPSocket();

        const TCPSocket* getSocket() const override;

        uint32_t send( const uint8_t* data, int32_t size ) const;
        uint32_t receive( uint8_t* data, int32_t size ) const;

        friend class Listener;

    #ifdef WIN32
        SOCKET getHandle() const;
    #else
        int getHandle() const;
    #endif

    private:
    #ifdef WIN32
        TCPSocket( const SOCKET& socket );
        SOCKET m_socket;
    #else
        TCPSocket( int socket );
        int m_socket;
    #endif
};



inline const TCPSocket* TCPSocket::getSocket() const
{
    return this;
}

#ifdef WIN32
inline SOCKET TCPSocket::getHandle() const
{
    return m_socket;
}

#else
inline int TCPSocket::getHandle() const
{
    return m_socket;
}
#endif
