#pragma once

#include <memory>

class Entity;

class Component
{
    public:
        // order of values specifies the order of initalization
        enum class Type
        {
            POSITION,
            ACTOR
        };

        virtual ~Component();

        std::shared_ptr< Entity > getEntity() const;

        virtual void init( const std::shared_ptr< Entity >& entity );
        virtual void destroy();

    private:
        std::weak_ptr< Entity > m_entity;
};



inline Component::~Component()
{ }

inline std::shared_ptr< Entity > Component::getEntity() const
{
    return m_entity.lock();
}

inline void Component::init( const std::shared_ptr< Entity >& entity )
{
    m_entity = entity;
}

inline void Component::destroy()
{ }
