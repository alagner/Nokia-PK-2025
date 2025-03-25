#pragma once

#include "BaseState.hpp"

namespace ue
{

class ConnectingState : public BaseState
{
public:
    ConnectingState(Context& context);

    void enter() override;

    // IBtsEventsHandler interface
    void handleAttachAccept() override;
    void handleAttachReject() override;

    // ITimerEventsHandler interface
    void handleTimeout() override;
};

}
