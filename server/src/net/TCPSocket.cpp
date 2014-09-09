#include "TCPSocket.hpp"

#include "Socket.hpp"

#include <iostream>

#ifdef WIN32

    TCPSocket::TCPSocket(
        const std::string& address,
        int port )
    {
        // create socket
        m_socket = ::socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
        if( m_socket == INVALID_SOCKET )
            throw Error::CREATE;

        // build connection
        sockaddr_in sockAddr;
        sockAddr.sin_family = AF_INET;
        sockAddr.sin_port = htons( port );
        sockAddr.sin_addr.S_un.S_addr = inet_addr( address.c_str() );
        if( connect( m_socket, reinterpret_cast< sockaddr* >( &sockAddr ), sizeof( sockAddr ) ) )
        {
            closesocket( m_socket );
            throw Error::CONNECT;
        }
    }

    TCPSocket::TCPSocket( const SOCKET& socket )
        : m_socket( socket )
    {
    }

    TCPSocket::~TCPSocket()
    {
        shutdown( m_socket, SD_SEND );
        //recv(m_socket, NULL, 0, 0);
        closesocket( m_socket );
    }

    uint32_t TCPSocket::send( const uint8_t* data, int32_t size ) const
    {
        int ret = ::send( m_socket, reinterpret_cast< const char* >( data ), size, 0 );

        if( ret == SOCKET_ERROR )
            throw Error::BROKEN;

        return ret;
    }

    uint32_t TCPSocket::receive( uint8_t* data, int32_t size ) const
    {
        while( true )
        {
            int ret = recv( m_socket, reinterpret_cast< char* >( data ), size, 0 );

            if( ret == SOCKET_ERROR )
            {
                int errorNum = WSAGetLastError();
                switch( errorNum )
                {
                    case WSAEWOULDBLOCK:
                        break;
                    case WSAECONNRESET:
                        throw Error::CLOSED;
                    default:
                        throw Error::BROKEN;
                }
            }
            else if( ret == 0 )
                throw Error::CLOSED;
            else
                return ret;
        }
    }

#else

    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <netdb.h>
    #include <unistd.h>
    #include <stdio.h>

    TCPSocket::TCPSocket(
        const std::string& address,
        int port )
    {
        // create socket
        m_socket = ::socket( AF_INET, SOCK_STREAM, 0 );
        if( m_socket == -1 )
            throw Error::CREATE;

        struct sockaddr_in sockAddr = Socket::createAddress( address, port );
        if( connect( m_socket, reinterpret_cast< struct sockaddr* >( &sockAddr ), sizeof( sockAddr ) ) )
        {
            close( m_socket );
            throw Error::CONNECT;
        }
    }

    TCPSocket::TCPSocket( int socket )
        : m_socket( socket )
    {
    }

    TCPSocket::~TCPSocket()
    {
        shutdown( m_socket, 0 );
        close( m_socket );
    }

    uint32_t TCPSocket::send( const uint8_t* data, int32_t size ) const
    {
        int ret = ::send( m_socket, reinterpret_cast< const char* >( data ), size, 0 );

        if( ret == -1 )
            throw Error::BROKEN;

        return ret;
    }

    uint32_t TCPSocket::receive( uint8_t* data, int32_t size ) const
    {
        while( true )
        {
            int ret = recv( m_socket, reinterpret_cast< void* >( data ), size, 0 );

            if( ret > 0 )
                return ret;
            else if( ret == 0 )
                throw Error::CLOSED;
            else
            {
                switch( errno )
                {
                    case EAGAIN:
                        break;
                    default:
                        throw Error::BROKEN;
                }
            }
        }
    }

#endif
