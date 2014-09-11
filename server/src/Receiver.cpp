#include "Receiver.hpp"

#include <Connection.hpp>

Receiver::Receiver()
    : m_running( true )
    , m_packetSelector(
        std::bind( &Receiver::netEvent, this, std::placeholders::_1 ) )
    , m_thread( std::bind( &Receiver::run, this ) )
{ }

Receiver::~Receiver()
{
    std::unique_lock< std::mutex > lock( m_mutex );
    m_running = false;
    m_packetSelector.notify();
    lock.unlock();

    m_thread.join();
}

void Receiver::add( std::shared_ptr< Connection > connection )
{
    m_packetSelector.add( std::move( connection ) );
}

void Receiver::remove( std::shared_ptr< Connection > connection )
{
    m_packetSelector.remove( std::move( connection ) );
}

void Receiver::netEvent( std::unique_ptr< PacketSelector::NetEvent > event )
{
    auto connection = std::static_pointer_cast< Connection >( event->getSource() );
    connection->netEvent( std::move( event ) );
}

void Receiver::run()
{
    std::unique_lock< std::mutex > lock( m_mutex );
    while( m_running )
    {
        lock.unlock();

        m_packetSelector.wait();

        lock.lock();
    }
}
