#include "Actor.hpp"

#include <game/Behaviour.hpp>
#include <game/Action.hpp>

using components::Actor;

Actor::Actor( std::unique_ptr< Behaviour > behaviour )
    : m_behaviour( std::move( behaviour ) )
{ }

void Actor::init(
    const std::shared_ptr< Entity >& entity )
{
    Component::init( entity );

    setAction( m_behaviour->nextAction() );
}

void Actor::setAction( std::shared_ptr< Action > action )
{
    /*
    if( m_action )
        m_action->interrupt( std::chrono::steady_clock::now() );
    */

    m_action = std::move( action );

    if( m_action )
        m_action->start(
            getEntity(),
            std::bind( &Actor::finishCallback, this ) );
}

void Actor::finishCallback()
{
    setAction( m_behaviour->nextAction() );
}
