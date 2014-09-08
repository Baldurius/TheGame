#pragma once

#include <string>

#ifndef WIN32
    #include <netinet/in.h>
    #include <sys/socket.h>
    #include <sys/types.h>
    #include <netdb.h>
#endif

class Socket
{
    public:
        static void init();
        static void destroy();

        #ifndef WIN32
            static struct sockaddr_in createAddress( const std::string& address, int port );
        #endif
};
