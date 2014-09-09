#include "PacketSelector.hpp"

#include "IPacket.hpp"
#include "SocketContainer.hpp"
#include "TCPSocket.hpp"

#include <iostream>

#ifdef WIN32

    PacketSelector::NetEvent::NetEvent( Type type, SocketContainer* source )
        : m_type( type )
        , m_source( source )
    {
    }

    PacketSelector::PacketSelector(
        std::function< void ( std::unique_ptr< NetEvent > ) callback )
        : m_callback( std::move( callback ) )
    {
        m_notifyEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
        m_events = new WSAEVENT[ 1 ];
        m_events[ 0 ] = m_notifyEvent;
        m_eventSize = 1;
    }

    PacketSelector::~PacketSelector()
    {
        delete[] m_events;

        for( std::tuple< HANDLE, SocketContainer* >& tuple : m_container )
            CloseHandle( std::get< 0 >( tuple ) );

        CloseHandle( m_notifyEvent );
    }

    void PacketSelector::wait( std::function< void ( std::unique_ptr< NetEvent > event ) > callback )
    {
        bool rebuildList = false;
        m_running = true;
        while( true )
        {
            int res = WSAWaitForMultipleEvents( m_eventSize, m_events, FALSE, WSA_INFINITE, FALSE );
            res -= WAIT_OBJECT_0;

            if( res == m_eventSize - 1 )
            {
                ResetEvent( m_notifyEvent );
                if( !m_running )
                    break;
                rebuildList = true;
            }
            else
            {
                m_containerMutex.lock();
                std::tuple< HANDLE, SocketContainer* > tup = m_container[ res ];
                m_containerMutex.unlock();

                ResetEvent( std::get< 0 >( tup ) );
                SocketContainer* container = std::get< 1 >( tup );

                try
                {
                    std::unique_ptr< IPacket > packet( IPacket::fromTCPSocket( container->getSocket() ) );

                    callback( std::unique_ptr< NetEvent >( new NetEvent_Impl< IPacket >(
                        NetEvent::Type::PACKET,
                        container,
                        std::move( packet ) ) ) );
                }
                catch( TCPSocket::Error& e )
                {
                    CloseHandle( std::get< 0 >( tup ) );
                    m_containerMutex.lock();
                    m_container.erase( m_container.begin() + res );
                    m_containerMutex.unlock();
                    rebuildList = true;

                    switch( e )
                    {
                        case TCPSocket::Error::CLOSED:
                            callback( std::unique_ptr< NetEvent >( new NetEvent( NetEvent::Type::CLOSED, container ) ) );
                            break;
                        case TCPSocket::Error::BROKEN:
                            callback( std::unique_ptr< NetEvent >( new NetEvent( NetEvent::Type::BROKEN, container ) ) );
                            break;
                        default:
                            break;
                    }
                }
            }

            if( rebuildList )
            {
                rebuildList = false;

                delete[] m_events;
                m_events = new WSAEVENT[ m_container.size() + 1 ];

                m_containerMutex.lock();
                {
                    // call connection events for new connections
                    for( int i = m_eventSize - 1; i < m_container.size(); ++i )
                    {
                        callback( std::unique_ptr< NetEvent >( new NetEvent(
                            NetEvent::Type::CONNECT,
                            std::get< 1 >( m_container[ i ] ) ) ) );
                    }

                    m_eventSize = m_container.size() + 1;

                    for( int i = 0; i < m_container.size(); ++i )
                        m_events[ i ] = std::get< 0 >( m_container[ i ] );
                }
                m_containerMutex.unlock();

                m_events[ m_eventSize - 1 ] = m_notifyEvent;
            }
        }
    }

    void PacketSelector::add( SocketContainer* container )
    {
        HANDLE event = CreateEvent( NULL, TRUE, FALSE, NULL );
        WSAEventSelect( container->getSocket()->handle(), event, FD_READ | FD_CLOSE );
        {
            std::unique_lock< std::mutex > lock( m_containerMutex );
            m_container.push_back( std::make_tuple( event, container ) );
        }
        SetEvent( m_notifyEvent );
    }

    void PacketSelector::notify()
    {
        m_running = false;
        SetEvent( m_notifyEvent );
    }

#else

    #include <sys/socket.h>
    #include <sys/types.h>
    #include <fcntl.h>
    #include <netdb.h>
    #include <unistd.h>
    #include <stdio.h>

    PacketSelector::NetEvent::NetEvent( Type type, SocketContainer* source )
        : m_type( type )
        , m_source( source )
    {
    }

    PacketSelector::PacketSelector(
        std::function< void ( std::unique_ptr< NetEvent > ) > callback )
        : m_callback( std::move( callback ) )
    {
        m_epoll = epoll_create1( 0 );

        if( m_epoll == -1 )
            std::cout << "EPOLL-INIT-ERROR: " << errno << std::endl;

        struct epoll_event event;
        event.events = EPOLLIN;
        event.data.ptr = NULL;
        pipe( m_pipe );

        if( epoll_ctl( m_epoll, EPOLL_CTL_ADD, m_pipe[ 0 ], &event ) == -1 )
            std::cout << "EPOLL-PIPE-ERROR: " << errno << std::endl;
    }

    PacketSelector::~PacketSelector()
    {
        notify();
        close( m_pipe[ 0 ] );
        close( m_pipe[ 1 ] );
    }

    void PacketSelector::wait()
    {
        while( true )
        {
            // process socket changes
            {
                std::unique_lock< std::mutex > lock( m_mutex );
                // add new sockets
                while( !m_addList.empty() )
                {
                    auto container = m_addList.front();

                    struct epoll_event event;
                    event.events = EPOLLIN;
                    event.data.ptr = container;
                    epoll_ctl( m_epoll, EPOLL_CTL_ADD, container->getSocket()->getHandle(), &event );

                    m_callback( std::unique_ptr< NetEvent >( new NetEvent(
                        NetEvent::Type::CONNECT, container ) ) );

                    m_addList.pop_front();
                }

                // remove socket handles
                while( !m_removeList.empty() )
                {
                    auto handle = m_removeList.front();

                    epoll_ctl( m_epoll, EPOLL_CTL_DEL, handle, NULL );

                    m_removeList.pop_front();
                }
            }

            int num = epoll_wait( m_epoll, m_events, 64, -1 );

            switch( num )
            {
                case 0: // no events
                    break;

                case -1: // an error occured
                {
                    switch( errno )
                    {
                        case EINTR:
                            break;
                        default:
                            std::cout << "EPOLL-ERROR: " << errno << std::endl;
                            return;
                    }
                } break;

                default: // events available
                {
                    for( int i = 0; i < num; ++i )
                    {
                        if( m_events[ i ].data.ptr == NULL )
                        {
                            char buff;
                            read( m_pipe[ 0 ], &buff, 1 );
                            if( buff == '1' )
                                return;
                        }
                        else
                        {
                            SocketContainer* container = static_cast< SocketContainer* >( m_events[ i ].data.ptr );

                            try
                            {
                                std::unique_ptr< IPacket > packet( IPacket::fromTCPSocket( container->getSocket() ) );

                                m_callback( std::unique_ptr< NetEvent >( new NetEvent_Impl< IPacket >(
                                    NetEvent::Type::PACKET,
                                    container,
                                    std::move( packet ) ) ) );
                            }
                            catch( TCPSocket::Error& e )
                            {
                                m_removeList.push_back( container->getSocket()->getHandle() );

                                switch( e )
                                {
                                    case TCPSocket::Error::CLOSED:
                                        m_callback( std::unique_ptr< NetEvent >( new NetEvent( NetEvent::Type::CLOSED, container ) ) );
                                        break;
                                    case TCPSocket::Error::BROKEN:
                                        m_callback( std::unique_ptr< NetEvent >( new NetEvent( NetEvent::Type::BROKEN, container ) ) );
                                        break;
                                    default:
                                        break;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    void PacketSelector::add( SocketContainer* container )
    {
        // set socket into nonblocking mode
        int flags = fcntl( container->getSocket()->getHandle(), F_GETFL, 0 );
        fcntl( container->getSocket()->getHandle(), F_SETFL, flags | O_NONBLOCK );

        // add socket to event list
        std::unique_lock< std::mutex > lock( m_mutex );

        for( auto i = m_removeList.begin(); i != m_removeList.end(); )
        {
            if( *i == container->getSocket()->getHandle() )
                i = m_removeList.erase( i );
            else
                ++i;
        }

        m_addList.push_back( container );

        // notify waiting thread
        char buff = '0';
        write( m_pipe[ 1 ], &buff, 1 );
    }

    void PacketSelector::remove( SocketContainer* container )
    {
        std::unique_lock< std::mutex > lock( m_mutex );

        for( auto i = m_addList.begin(); i != m_addList.end(); )
        {
            if( *i == container )
                i = m_addList.erase( i );
            else
                ++i;
        }

        m_removeList.push_back( container->getSocket()->getHandle() );

        // notify waiting thread
        char buff = '0';
        write( m_pipe[ 1 ], &buff, 1 );
    }

    void PacketSelector::notify()
    {
        char buff = '1';
        write( m_pipe[ 1 ], &buff, 1 );
    }

#endif
