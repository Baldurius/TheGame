#include "Acceptor.hpp"

#include <net/Listener.hpp>
#include <net/TCPSocket.hpp>

Acceptor::Acceptor(
    uint16_t port,
    std::function< void ( std::unique_ptr< TCPSocket > ) > callback )
    : m_running( true )
    , m_port( port )
    , m_callback( std::move( callback ) )
    , m_thread( std::bind( &Acceptor::run, this ) )
{
}

Acceptor::~Acceptor()
{
    std::unique_lock< std::mutex > lock( m_mutex );
    m_running = false;
    lock.unlock();

    // TODO: interrupt the accepting block
    // m_thread.join();
}

void Acceptor::run()
{
    std::unique_ptr< Listener > listener( new Listener( m_port ) );

    // locking should be unnecessary because m_running has atomic read/write
    std::unique_lock< std::mutex > lock( m_mutex );
    while( m_running )
    {
        lock.unlock();

        // no lock needed because listener and m_callback never change
        auto socket = listener->accept();
        m_callback( std::move( socket ) );

        lock.lock();
    }
}

