#pragma once

#include <game/Component.hpp>

#include <map>

class Game;

class Entity
    : public std::enable_shared_from_this< Entity >
{
    public:
        template< class T, class... Args >
        class Creator;

        virtual ~Entity();

        template< class T >
        std::shared_ptr< T > getComponent() const;

        std::shared_ptr< Game > getGame() const;

        virtual void init();
        virtual void destroy();

    protected:
        Entity( const std::shared_ptr< Game >& game );

        template< class T >
        void addComponent( std::shared_ptr< T > component );

    private:
        std::map< Component::Type, std::shared_ptr< Component > > m_components;

        std::weak_ptr< Game > m_game;
};

template< class T, class... Args >
class Entity::Creator
{
    public:
        static std::shared_ptr< T > create(
            const std::shared_ptr< Game >& game,
            Args... args );
};


inline Entity::Entity( const std::shared_ptr< Game >& game )
    : m_game( game )
{ }

inline Entity::~Entity()
{ }

template< class T >
inline std::shared_ptr< T > Entity::getComponent() const
{
    auto iter = m_components.find( T::getType() );
    return iter == m_components.end()
        ? std::shared_ptr< T >()
        : std::static_pointer_cast< T >( iter->second );
}

inline std::shared_ptr< Game > Entity::getGame() const
{
    return m_game.lock();
}

template< class T >
inline void Entity::addComponent( std::shared_ptr< T > component )
{
    m_components[ T::getType() ] = std::move( component );
}

inline void Entity::init()
{
    auto thisPtr = shared_from_this();
    for( auto compIter : m_components )
        compIter.second->init( thisPtr );
}

inline void Entity::destroy()
{
    for( auto compIter : m_components )
        compIter.second->destroy();

    m_components.clear();
}

template< class T, class... Args >
inline std::shared_ptr< T > Entity::Creator< T, Args... >::create(
    const std::shared_ptr< Game >& game,
    Args... args )
{
    std::shared_ptr< T > result( new T( game, args... ) );
    result->init();
    return std::move( result );
}
