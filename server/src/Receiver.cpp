#include "Receiver.hpp"

Receiver::Receiver(
    std::unique_ptr< PacketSelector > selector,
    std::function< void ( std::unique_ptr< PacketSelector::NetEvent > ) > callback )
    : m_running( true )
    , m_selector( std::move( selector ) )
    , m_callback( std::move( callback ) )
    , m_thread( std::bind( &Receiver::run, this ) )
{ }

Receiver::~Receiver()
{
    std::unique_lock< std::mutex > lock( m_mutex );
    m_running = false;
    m_selector->notify();
}

void Receiver::run()
{
    std::unique_lock< std::mutex > lock( m_mutex );
    while( m_running )
    {
        lock.unlock();

        // no need to protect selector and callback since they never change
        m_selector->wait( m_callback );

        lock.lock();
    }
}
