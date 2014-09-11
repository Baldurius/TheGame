#include "Lobby.hpp"

#include <Client.hpp>
#include <net/IPacket.hpp>
#include <net/TCPSocket.hpp>
#include <constants.hpp>
#include <game/Game.hpp>
#include <Connection.hpp>

#include <iostream>

void Lobby::add( std::unique_ptr< Client > client )
{
    std::cout << client->getName() << " joined the lobby!" << std::endl;
    client->getConnection()->setCallback(
        std::function< void ( std::unique_ptr< PacketSelector::NetEvent > ) >() );
    m_clients.push_back( std::move( client ) );
}
