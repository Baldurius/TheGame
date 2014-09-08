#include "Listener.hpp"

#include "TCPSocket.hpp"

#ifdef WIN32

    Listener::Listener( int port )
    {
        // create socket
        m_socket = ::socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
        if( m_socket == INVALID_SOCKET )
            throw Error::CREATE;

        // build connection
        sockaddr_in sockAddr;
        sockAddr.sin_family = AF_INET;
        sockAddr.sin_port = htons( port );
        sockAddr.sin_addr.S_un.S_addr = INADDR_ANY;
        if( bind( m_socket, reinterpret_cast< sockaddr* >( &sockAddr ), sizeof( sockAddr ) ) )
        {
            closesocket( m_socket );
            throw Error::BIND;
        }

        if( listen( m_socket, 10 ) )
        {
            closesocket( m_socket );
            throw Error::LISTEN;
        }
    }

    Listener::~Listener()
    {
        closesocket( m_socket );
    }

    std::unique_ptr< TCPSocket > Listener::accept()
    {
        SOCKET socket = ::accept( m_socket, NULL, NULL );
        if( socket == INVALID_SOCKET )
            throw Error::ACCEPT;
        return std::unique_ptr< TCPSocket >( new TCPSocket( socket ) );
    }

#else

    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <sys/types.h>
    #include <netdb.h>
    #include <unistd.h>
    #include <stdio.h>

    Listener::Listener( int port )
    {
        // create socket
        m_socket = ::socket( AF_INET, SOCK_STREAM, 0 );
        if( m_socket == -1 )
            throw Error::CREATE;

        // build connection
        struct sockaddr_in sockAddr;
        sockAddr.sin_family = AF_INET;
        sockAddr.sin_port = htons( port );
        sockAddr.sin_addr.s_addr = INADDR_ANY;
        if( bind( m_socket, reinterpret_cast< struct sockaddr* >( &sockAddr ), sizeof( sockAddr ) ) )
        {
            close( m_socket );
            throw Error::BIND;
        }

        if( listen( m_socket, 10 ) )
        {
            close( m_socket );
            throw Error::LISTEN;
        }
    }

    Listener::~Listener()
    {
        close( m_socket );
    }

    std::unique_ptr< TCPSocket > Listener::accept()
    {
        int socket = ::accept( m_socket, NULL, NULL );
        if( socket == -1 )
            throw Error::ACCEPT;
        return std::unique_ptr< TCPSocket >( new TCPSocket( socket ) );
    }

#endif
