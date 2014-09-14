#pragma once

#include <game/Action.hpp>

#include <chrono>

class CallbackEvent;

namespace actions
{
    class MoveLinear
        : public Action
    {
        public:
            typedef std::chrono::steady_clock::duration Duration;

            MoveLinear(
                float dstX, float dstY, Duration duration );

            // Action
            virtual void start(
                const std::shared_ptr< Entity >& entity,
                std::function< void() > callback ) override;

            void finishCallback();

        private:
            float m_dstX;
            float m_dstY;
            Duration m_duration;

            std::shared_ptr< CallbackEvent > m_finishEvent;
    };
}
