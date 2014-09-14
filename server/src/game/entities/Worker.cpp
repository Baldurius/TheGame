#include "Worker.hpp"

#include <game/components/Position.hpp>
#include <game/components/Actor.hpp>
#include <game/Behaviour.hpp>
#include <game/actions/Wait.hpp>
#include <game/actions/MoveLinear.hpp>

#include <iostream>

using entities::Worker;

namespace
{
    class BehaviourImpl
        : public Behaviour
    {
        public:
            virtual std::shared_ptr< Action > nextAction() override
            {
                return std::make_shared< actions::MoveLinear >( 30, 30,
                    std::chrono::milliseconds( 3000 ) );
            };
    };
}

Worker::Worker(
    const std::shared_ptr< Game >& game,
    float x,
    float y )
    : Entity( game )
{
    addComponent( std::make_shared< components::Position >(
        x, y ) );
    addComponent( std::make_shared< components::Actor >(
        std::unique_ptr< Behaviour >( new BehaviourImpl() ) ) );
}
