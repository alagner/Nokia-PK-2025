#pragma once

#include "BaseState.hpp"

namespace ue
{

class ConnectingState : public BaseState
{
public:
    ConnectingState(Context& context);

    // IBtsEventsHandler interface
    void handleAttachAccept() override;
    void handleAttachReject() override;
    void handleDisconnect() override;

    // ITimerEventsHandler interface
    void handleTimeout() override;

    void handleTalkMessage(common::PhoneNumber from, const std::string& text) override;
};

}
