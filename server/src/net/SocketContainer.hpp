#pragma once

class TCPSocket;

class SocketContainer
{
    public:
        virtual ~SocketContainer() { }

        virtual const TCPSocket* getSocket() const = 0; 
};
