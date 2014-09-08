#include "Socket.hpp"

#ifdef WIN32

    #include <winsock2.h>

    void Socket::init()
    {
        const int vers = 2;
        WSADATA wsaData;
        auto succ = WSAStartup( MAKEWORD( vers, 0 ), &wsaData );
/*
        if( !succ )
            logger( tg::Logger::Mode::ERROR ) << "Unable to initialize Winsock." << std::endl;
*/
    }

    void Socket::destroy()
    {
        WSACleanup();
/*
        if( WSACleanup() )
            logger( tg::Logger::Mode::ERROR ) << "Unable to destroy Winsock." << std::endl;
*/
    }

#else

    #include <mutex>
    #include <string.h>

    void Socket::init()
    {
    }

    void Socket::destroy()
    {
    }

    struct sockaddr_in Socket::createAddress( const std::string& address, int port )
    {
        static std::mutex mutex;

        std::unique_lock< std::mutex > lock( mutex );

        struct hostent* host = gethostbyname( address.c_str() );
        struct sockaddr_in sockAddr;
        sockAddr.sin_family = AF_INET;
        sockAddr.sin_port = htons( port );
        bcopy( reinterpret_cast< char* >( host->h_addr ), reinterpret_cast< char* >( &sockAddr.sin_addr.s_addr ), host->h_length );

        return sockAddr;
    }    

#endif
