#include "Application.hpp"

#include <net/OPacket.hpp>
#include <net/IPacket.hpp>
#include <net/Listener.hpp>
#include <net/TCPSocket.hpp>
#include <net/Socket.hpp>

#include <iostream>

int main( int argc, char** argv )
{
    // convert arguments to string vector
    std::vector< std::string > arguments( argc - 1 );
    for( int i = 0; i < argc - 1; ++i )
        arguments.push_back( argv[ i + 1 ] );

    // create main application
    std::unique_ptr< Application > app(
        new Application( std::move( arguments ) ) );

    // run application
    app->run();

    return 0;
}

Application::Application( std::vector< std::string > arguments )
{ }

void Application::run()
{
    Socket::init();

    short int number = 0x1;
    char *numPtr = (char*)&number;
    if( numPtr[ 0 ] == 1 )
        std::cout << "Little endian" << std::endl;
    else
        std::cout << "Big endian" << std::endl;

    std::unique_ptr< Listener > listener( new Listener( 7777 ) );

    auto socket = listener->accept();

    std::cout << "New connection" << std::endl;

    {
        OPacket packet;
        packet.write< std::string >( "Dies ist ein Test\n" );
        packet.send( socket.get() );
        std::cout << "Sending..." << std::endl;
    }

    {
        std::unique_ptr< IPacket > packet( IPacket::fromTCPSocket( socket.get() ) );
        std::cout << "Received data: " << packet->read< std::string >() << std::endl;
    }

    Socket::destroy();
}
