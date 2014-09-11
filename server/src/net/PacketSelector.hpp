#pragma once

#ifdef WIN32
    #include <winsock2.h>
#else
    #include <sys/epoll.h>
#endif

#include <functional>
#include <vector>
#include <mutex>
#include <memory>
#include <list>

class IPacket;
class SocketContainer;
class TCPSocket;

class PacketSelector
{
    public:
        class NetEvent;

        template< class T >
        class NetEvent_Impl;

        PacketSelector(
            std::function< void ( std::unique_ptr< NetEvent > ) > callback );

        ~PacketSelector();

        void wait();

        void add( std::shared_ptr< SocketContainer > container );
        void remove( std::shared_ptr< SocketContainer > container );

        void notify();

    private:
        enum class Change
        {
            ADD,
            REMOVE
        };

        bool m_running;
        std::function< void ( std::unique_ptr< NetEvent > ) > m_callback;

    #ifdef WIN32
        std::mutex m_containerMutex;
        std::vector< std::tuple< HANDLE, SocketContainer* > > m_container;
        WSAEVENT* m_events;
        int m_eventSize;

        HANDLE m_notifyEvent;
    #else
        int m_pipe[ 2 ];
        int m_epoll;
        struct epoll_event m_events[ 64 ];
        std::mutex m_mutex;

        std::list< std::tuple<
            Change,
            std::shared_ptr< SocketContainer >* > > m_changeList;
    #endif
};



class PacketSelector::NetEvent
{
    public:
        enum class Type
        {
            PACKET,
            CLOSED,
            BROKEN,
            CONNECT
        };

        NetEvent( Type type, std::shared_ptr< SocketContainer > source );

        virtual ~NetEvent() { }

        Type getType() const;

        const std::shared_ptr< SocketContainer >& getSource() const;

        template< class T >
        std::unique_ptr< T > getData();

    private:
        Type m_type;
        std::shared_ptr< SocketContainer > m_source;
};

template< class T >
class PacketSelector::NetEvent_Impl : public PacketSelector::NetEvent
{
    public:
        NetEvent_Impl(
            Type type,
            std::shared_ptr< SocketContainer > source,
            std::unique_ptr< T > data );

        std::unique_ptr< T > getData();

    private:
        std::unique_ptr< T > m_data;
};



inline PacketSelector::NetEvent::Type PacketSelector::NetEvent::getType() const
{
    return m_type;
}

inline const std::shared_ptr< SocketContainer >& PacketSelector::NetEvent::getSource() const
{
    return m_source;
}

template< class T >
inline std::unique_ptr< T > PacketSelector::NetEvent::getData()
{
    NetEvent_Impl< T >* th = static_cast< NetEvent_Impl< T >* >( this );
    return th
        ? th->getData()
        : std::unique_ptr< T >();
}

template< class T >
inline PacketSelector::NetEvent_Impl< T >::NetEvent_Impl(
    Type type,
    std::shared_ptr< SocketContainer > source,
    std::unique_ptr< T > data )
    : NetEvent( type, std::move( source ) )
    , m_data( std::move( data ) )
{
}

template< class T >
inline std::unique_ptr< T > PacketSelector::NetEvent_Impl< T >::getData()
{
    return std::move( m_data );
}
