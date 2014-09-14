#pragma once

#include <game/Component.hpp>

class Behaviour;
class Action;

namespace components
{
    class Actor
        : public Component
    {
        public:
            static Component::Type getType();

            Actor( std::unique_ptr< Behaviour > behaviour );

            virtual void init(
                const std::shared_ptr< Entity >& entity ) override;

            void setAction( std::shared_ptr< Action > action );

            void finishCallback();

        private:
            std::unique_ptr< Behaviour > m_behaviour;

            std::shared_ptr< Action > m_action;
    };



    inline Component::Type Actor::getType()
    {
        return Component::Type::ACTOR;
    }
}
