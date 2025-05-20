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
    void sendTalkMessage(const std::string& text);
    void handleTalkMessage(common::PhoneNumber from, const std::string& text);

};

}

#endif  // TALKINGSTATE_H
