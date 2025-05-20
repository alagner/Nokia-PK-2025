#ifndef TALKINGSTATE_H
#define TALKINGSTATE_H
#pragma once

#include "States/BaseState.hpp"

namespace ue {

class TalkingState : public BaseState
{
   public:
    common::PhoneNumber to;
    explicit TalkingState(Context& context, common::PhoneNumber to);
    void handleDisconnect() override;
    void sendTalkMessage(common::PhoneNumber to, const std::string& text) override;
};

}

#endif  // TALKINGSTATE_H
