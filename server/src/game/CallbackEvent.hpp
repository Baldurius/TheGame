#pragma once

#include <game/Event.hpp>

class CallbackEvent
    : public Event
{
    public:
        CallbackEvent(
            TimePoint timePoint,
            std::function< void () > callback );

        virtual void execute() override;

    private:
        std::function< void () > m_callback;
};
