#include "Application.hpp"

#include <net/OPacket.hpp>
#include <net/IPacket.hpp>
#include <net/Listener.hpp>
#include <net/TCPSocket.hpp>
#include <net/Socket.hpp>
#include <Client.hpp>
#include <Authenticator.hpp>
#include <Acceptor.hpp>
#include <Lobby.hpp>

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

    std::unique_ptr< Lobby > lobby( new Lobby() );

    std::unique_ptr< Authenticator > authenticator( new Authenticator(
        std::bind( &Lobby::add, lobby.get(), std::placeholders::_1 ) ) );

    std::unique_ptr< Acceptor > acceptor( new Acceptor(
        7777,
        std::bind( &Authenticator::add, authenticator.get(), std::placeholders::_1 ) ) );

    while( true )
    { }

    Socket::destroy();
}
