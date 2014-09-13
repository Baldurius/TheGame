#pragma once

#include <chrono>

class Event
{
    public:
        typedef std::chrono::steady_clock::time_point TimePoint;

        static bool compare(
            const std::shared_ptr< Event >& event1,
            const std::shared_ptr< Event >& event2 );

        Event( TimePoint timePoint );
        virtual ~Event();

        bool isValid() const;
        const TimePoint& getTimePoint() const;

        void invalidate();

        virtual void execute() = 0;

    private:
        bool m_valid;
        TimePoint m_timePoint;
};



inline bool Event::compare(
    const std::shared_ptr< Event >& event1,
    const std::shared_ptr< Event >& event2 )
{
    // heap always sorts the biggest element to front but
    // wants the comparator to return, whether first element is less
    return event1->getTimePoint() > event2->getTimePoint();
}

inline Event::Event( TimePoint timePoint )
    : m_valid( true )
    , m_timePoint( std::move( timePoint ) )
{ }

inline Event::~Event()
{ }

inline bool Event::isValid() const
{
    return m_valid;
}

inline const Event::TimePoint& Event::getTimePoint() const
{
    return m_timePoint;
}

inline void Event::invalidate()
{
    m_valid = false;
}
