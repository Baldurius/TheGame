#pragma once

#include <net/PacketSelector.hpp>

#include <memory>
#include <mutex>

class Application
{
    public:
        friend int main( int argc, char** argv );

    private:
        Application( std::vector< std::string > arguments );
        void run();
};
