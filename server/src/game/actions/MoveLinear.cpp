#include "MoveLinear.hpp"

#include <game/Entity.hpp>
#include <game/Game.hpp>
#include <game/CallbackEvent.hpp>
#include <game/components/Position.hpp>

using actions::MoveLinear;

MoveLinear::MoveLinear(
    float dstX, float dstY, Duration duration )
    : m_dstX( dstX )
    , m_dstY( dstY )
    , m_duration( duration )
{ }

void MoveLinear::start(
    const std::shared_ptr< Entity >& entity,
    std::function< void() > callback )
{
    Action::start( entity, std::move( callback ) );

    // TODO: set movement of bounding volume

    m_finishEvent = std::make_shared< CallbackEvent >(
        std::chrono::steady_clock::now() + m_duration,
        std::bind( &MoveLinear::finishCallback, this ) );

    entity->getGame()->addEvent( m_finishEvent );
}

void MoveLinear::finishCallback()
{
    m_finishEvent.reset();

    auto entity = getEntity();
    if( entity )
    {
        auto position = entity->getComponent< components::Position >();
        if( position )
        {
            position->setX( m_dstX );
            position->setY( m_dstY );
        }
    }

    finish();
}
