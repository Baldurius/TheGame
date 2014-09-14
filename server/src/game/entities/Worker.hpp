#pragma once

#include <game/Entity.hpp>

namespace entities
{
    class Worker
        : public Entity
    {
        public:
            typedef Creator< Worker, float, float > Creator;
            friend Creator;

        private:
            Worker(
                const std::shared_ptr< Game >& game,
                float x,
                float y );
    };
}
