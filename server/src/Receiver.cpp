#include "Receiver.hpp"

Receiver::Receiver(
    std::function< void ( std::unique_ptr< PacketSelector::NetEvent > ) > callback )
    : m_running( true )
    , m_packetSelector( std::move( callback ) )
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

void Receiver::add( SocketContainer* container )
{
    m_packetSelector.add( container );
}

void Receiver::remove( SocketContainer* container )
{
    m_packetSelector.remove( container );
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
