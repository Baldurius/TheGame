#pragma once

#include <string>

class TCPSocket;

class IPacket
{
    public:
        static IPacket* fromTCPSocket( const TCPSocket* socket );
        ~IPacket();

        template< typename T >
        T read();

        void read( uint8_t* data, uint32_t size );

        void close();

        uint32_t getRemaining() const;

    private:
        IPacket() { }

        uint8_t* m_buffer;
        uint32_t m_size;
        uint32_t m_cursor;
};



template< typename T >
T IPacket::read()
{
    T value;
    read( reinterpret_cast< uint8_t* >( &value ), sizeof( T ) );
    return std::move( value );
}

template<>
std::string IPacket::read< std::string >();

inline void IPacket::close()
{
    m_cursor = m_size;
}

inline uint32_t IPacket::getRemaining() const
{
    return m_size - m_cursor;
}
