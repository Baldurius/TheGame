#include "Application.hpp"

#include <net/PacketSelector.hpp>
#include <net/TCPSocket.hpp>
#include <net/OPacket.hpp>
#include <net/IPacket.hpp>
#include <net/Socket.hpp>
#include <net/Listener.hpp>
#include <Receiver.hpp>

#include <iostream>

void receive( std::unique_ptr< PacketSelector::NetEvent > event )
{
    switch( event->getType() )
    {
        case PacketSelector::NetEvent::Type::BROKEN:
            std::cout << "Connection broken!" << std::endl;
            break;
        case PacketSelector::NetEvent::Type::CONNECT:
            std::cout << "New connection!" << std::endl;
            break;
        case PacketSelector::NetEvent::Type::PACKET:
        {
            std::cout << "Packet received" << std::endl;
            auto packet = event->getData< IPacket >();
            std::cout << "   Content: " << packet->read< std::string >() << std::endl;
        } break;
        case PacketSelector::NetEvent::Type::CLOSED:
            std::cout << "Connection closed" << std::endl;
            break;
    }

}

int main( int argc, char** argv )
{
    Socket::init();

    uint16_t port = 7777;
    if( argc > 1 )
        port = atoi( argv[ 1 ] );

    auto listener = std::make_shared< Listener >( port ); 

    std::cout << "Waiting for client on port " << port << "..." << std::endl;

    auto socket = listener->accept();

    std::unique_ptr< PacketSelector > selector( new PacketSelector() );
    selector->add( socket.get() );

    std::unique_ptr< Receiver > receiver( new Receiver(
        std::move( selector ),
        receive ) );

    { // sending
        std::cout << "Sending..." << std::endl;
        std::unique_ptr< OPacket > packet( new OPacket() );
        packet->write( std::string( "Hallo! Ich bin der Server!\n" ) );
        packet->send( socket.get() );
    }

    while( true )
    { }

    Socket::destroy();
    return 0;
}
