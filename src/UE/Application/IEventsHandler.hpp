#pragma once

#include "Ports/IBtsPort.hpp"
#include "Ports/IUserPort.hpp"
#include "Ports/ITimerPort.hpp"

namespace ue
{

    class IEventsHandler : public IBtsEventsHandler,
                           public ITimerEventsHandler,
                           public IUserEventsHandler
    {
    public:
        virtual ~IEventsHandler() = default;
    };

}
