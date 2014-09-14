#include "Worker.hpp"

#include <game/components/Position.hpp>

#include <iostream>

using entities::Worker;

Worker::Worker(
    const std::shared_ptr< Game >& game,
    float x,
    float y )
    : Entity( game )
{
    addComponent( std::make_shared< components::Position >(
        x, y ) );
}
