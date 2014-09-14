#pragma once

#include <memory>

class Entity;

class Action
{
    public:
        virtual ~Action();

        std::shared_ptr< Entity > getEntity() const;

        virtual void start(
            const std::shared_ptr< Entity >& entity,
            std::function< void () > callback ) = 0;

    protected:
        void finish();

    private:
        std::weak_ptr< Entity > m_entity;
        std::function< void () > m_callback;
};



inline Action::~Action()
{ }

inline std::shared_ptr< Entity > Action::getEntity() const
{
    return m_entity.lock();
}

inline void Action::finish()
{
    if( m_callback )
        m_callback();
}
