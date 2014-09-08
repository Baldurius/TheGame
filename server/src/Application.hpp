#pragma once

#include <net/PacketSelector.hpp>

#include <memory>
#include <mutex>

class Application
{
    public:
        friend int main( int argc, char** argv );

        void run();

        void receive( std::unique_ptr< PacketSelector::NetEvent > event );

    private:
        std::list< std::unique_ptr< IPacket > > m_packets;
        mutable std::mutex m_mutex;
};
