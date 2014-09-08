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

    PacketSelector::PacketSelector()
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

    PacketSelector::PacketSelector()
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
        close( m_pipe[ 0 ] );
        close( m_pipe[ 1 ] );
    }

    void PacketSelector::wait( std::function< void ( std::unique_ptr< NetEvent > event ) > callback )
    {
        bool rebuildList = false;
        m_running = true;
        while( true )
        {
            int num = epoll_wait( m_epoll, m_events, 64, -1 );

            if( num == -1 )
            {
                switch( errno )
                {
                    case EINTR:
                        break;
                    default:
                        std::cout << "EPOLL-ERROR: " << errno << std::endl;
                }
            }

            bool breakLoop = false;

            for( int i = 0; i < num; ++i )
            {
                if( m_events[ i ].data.ptr == NULL )
                {
                    char buff;
                    read( m_pipe[ 0 ], &buff, 1 );

                    if( !m_running )
                        breakLoop = true;
                }
                else
                {
                    SocketContainer* container = static_cast< SocketContainer* >( m_events[ i ].data.ptr );

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
                        epoll_ctl( m_epoll, EPOLL_CTL_DEL, container->getSocket()->handle(), NULL );

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
            }

            // add new sockets
            m_containerMutex.lock();
            for( auto i = m_newContainer.begin(); i != m_newContainer.end(); i = m_newContainer.erase( i ) )
            {
                {
                    struct epoll_event event;
                    event.events = EPOLLIN;
                    event.data.ptr = *i;
                    epoll_ctl( m_epoll, EPOLL_CTL_ADD, (*i)->getSocket()->handle(), &event );
                }

                callback( std::unique_ptr< NetEvent >( new NetEvent(
                    NetEvent::Type::CONNECT,
                    *i ) ) );
            }
            m_containerMutex.unlock();

            if( breakLoop )
                break;
        }
    }

    void PacketSelector::add( SocketContainer* container )
    {
        int flags = fcntl( container->getSocket()->handle(), F_GETFL, 0 );
        flags |= O_NONBLOCK;
        fcntl( container->getSocket()->handle(), F_SETFL, flags );
        {
            std::unique_lock< std::mutex > lock( m_containerMutex );
            m_newContainer.push_back( container );
        }
        char buff = ' ';
        write( m_pipe[ 1 ], &buff, 1 );
    }

    void PacketSelector::notify()
    {
        m_running = false;
        char buff = ' ';
        write( m_pipe[ 1 ], &buff, 1 );
    }

#endif
