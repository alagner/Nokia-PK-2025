#pragma once

#include "BaseState.hpp"
#include "Messages/PhoneNumber.hpp"

namespace ue
{

class TalkingState : public BaseState
{
public:
    TalkingState(Context& context, common::PhoneNumber peerNumber);
    ~TalkingState() override = default;

    void handleUiBack() override;
    void handleCallEnd(common::PhoneNumber peer) override;
    void handleDisconnected() override;

private:
    common::PhoneNumber peerPhoneNumber;
};

}