#include "Acceptor.hpp"

#include <net/Listener.hpp>
#include <net/TCPSocket.hpp>
#include <Receiver.hpp>
#include <Connection.hpp>

Acceptor::Acceptor(
    uint16_t port,
    std::shared_ptr< Receiver > receiver,
    std::function< void ( std::shared_ptr< Connection > ) > callback )
    : m_running( true )
    , m_port( port )
    , m_receiver( std::move( receiver ) )
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
        auto connection = std::make_shared< Connection >(
            m_receiver, std::move( socket ) );
        m_receiver->add( connection );
        m_callback( std::move( connection ) );

        lock.lock();
    }
}

