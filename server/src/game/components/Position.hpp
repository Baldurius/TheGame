#pragma once

#include <game/Component.hpp>

namespace components
{
    class Position
        : public Component
    {
        public:
            static Component::Type getType();

            Position( float x, float y );

            float getX() const;
            float getY() const;

            void setX( float x );
            void setY( float y );

        private:
            float m_x;
            float m_y;
    };



    inline Component::Type Position::getType()
    {
        return Component::Type::POSITION;
    }

    inline Position::Position( float x, float y )
        : m_x( x )
        , m_y( y )
    { }

    inline float Position::getX() const
    {
        return m_x;
    }

    inline float Position::getY() const
    {
        return m_y;
    }

    inline void Position::setX( float x )
    {
        m_x = x;
    }

    inline void Position::setY( float y )
    {
        m_y = y;
    }
}
