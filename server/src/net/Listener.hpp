#pragma once

#ifdef WIN32
    #include <winsock2.h>
#endif

#include <memory>

class TCPSocket;

class Listener
{
    public:
        enum class Error
        {
            CREATE,
            BIND,
            LISTEN,
            ACCEPT
        };

        Listener( int port );

        ~Listener();

        std::unique_ptr< TCPSocket > accept();

    private:
        #ifdef WIN32
            SOCKET m_socket;
        #else
            int m_socket;
        #endif
};
