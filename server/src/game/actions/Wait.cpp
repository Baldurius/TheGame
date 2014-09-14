#include "Wait.hpp"

#include <game/Entity.hpp>
#include <game/Game.hpp>
#include <game/CallbackEvent.hpp>

#include <iostream>

using actions::Wait;

Wait::Wait( std::chrono::steady_clock::duration duration )
    : m_duration( std::move( duration ) )
{ }

void Wait::start(
    const std::shared_ptr< Entity >& entity,
    std::function< void() > callback )
{
    Action::start( entity, std::move( callback ) );

    // TODO: set movement of bounding volume

    m_finishEvent = std::make_shared< CallbackEvent >(
        std::chrono::steady_clock::now() + m_duration,
        std::bind( &Wait::finishCallback, this ) );

    entity->getGame()->addEvent( m_finishEvent );
}

void Wait::finishCallback()
{
    m_finishEvent.reset();

    std::cout << "Finish!" << std::endl;

    finish();
}
