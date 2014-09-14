#include "CallbackEvent.hpp"

CallbackEvent::CallbackEvent(
    TimePoint timePoint,
    std::function< void () > callback )
    : Event( std::move( timePoint ) )
    , m_callback( std::move( callback ) )
{ }

void CallbackEvent::execute()
{
    m_callback();
}
