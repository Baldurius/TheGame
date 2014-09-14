#pragma once

#include <game/Action.hpp>

#include <chrono>

class CallbackEvent;

namespace actions
{
    class Wait
        : public Action
    {
        public:
            Wait( std::chrono::steady_clock::duration duration );

            // Action
            virtual void start(
                const std::shared_ptr< Entity >& entity,
                std::function< void() > callback ) override;

            void finishCallback();

        private:
            std::chrono::steady_clock::duration m_duration;

            std::shared_ptr< CallbackEvent > m_finishEvent;
    };
}
