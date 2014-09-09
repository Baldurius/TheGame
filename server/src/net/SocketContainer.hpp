#pragma once

class TCPSocket;

class SocketContainer
{
    public:
        virtual ~SocketContainer() { }

        virtual TCPSocket* getSocket() = 0; 
};
