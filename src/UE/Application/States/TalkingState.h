#ifndef TALKINGSTATE_H
#define TALKINGSTATE_H
#pragma once

#include "States/BaseState.hpp"

namespace ue {

class TalkingState : public BaseState
{
   public:
    explicit TalkingState(Context& context, common::PhoneNumber to);
    void handleDisconnect() override;
    void handleSms(common::PhoneNumber, std::string) override;
    void sendTalkMessage(const std::string& text);
    void handleTalkMessage(common::PhoneNumber from, const std::string& text);
    void handleTimeout();
    void handleCallRecipientNotAvailable();
    void handleRedirect();
    void callDrop() override;
    void handleCallDropped() override;
    void handleCallRequest(common::PhoneNumber from) override;

   private:
    common::PhoneNumber to;
};

}

#endif  // TALKINGSTATE_H
