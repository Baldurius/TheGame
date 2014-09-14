#pragma once

class Action;

class Behaviour
{
    public:
        virtual ~Behaviour();

        virtual std::shared_ptr< Action > nextAction() = 0;
};



inline Behaviour::~Behaviour()
{ }
