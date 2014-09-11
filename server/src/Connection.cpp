#include "Connection.hpp"

#include <Receiver.hpp>

void Connection::destroy()
{
    m_receiver.lock()->remove( shared_from_this() );
}
