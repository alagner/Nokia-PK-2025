#pragma once

#include "BaseState.hpp"
#include <chrono>

namespace ue
{

class TalkingState : public BaseState
{
public:
    TalkingState(Context& context, common::PhoneNumber phoneNumber);
    ~TalkingState() override;
    
    void handleDisconnect() override;
    void handleCallTalk(common::PhoneNumber from, std::string text) override;
    void handleCallDropped(common::PhoneNumber from) override;
    void handleTimeout() override;
    void acceptCallRequest() override;
    void rejectDialing() override;
    void rejectCallRequest() override;
    
    void handleUnknownRecipient(common::PhoneNumber to);
    
private:
    void startInactivityTimer();
    void resetInactivityTimer();
    void sendCallMessage(const std::string& message);
    void checkInactivity();
    
    common::PhoneNumber peerPhoneNumber;
    bool lastActivityWasReceive = false;
    std::chrono::steady_clock::time_point lastActivityTime;
    static constexpr std::chrono::minutes INACTIVITY_TIMEOUT{2};
};

}