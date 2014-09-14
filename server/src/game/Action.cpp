#include "Action.hpp"

void Action::start(
    const std::shared_ptr< Entity >& entity,
    std::function< void () > callback )
{
    m_entity = entity;
    m_callback = std::move( callback );
}
