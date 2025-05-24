#pragma once

#include "BaseState.hpp"

namespace ue
{

class DialState : public BaseState
{
public:
    DialState(Context& context);
    void handleTimeout() override;
    void handleDisconnect() override;
    void handleCallRequest(common::PhoneNumber from) override;
    void handleClose() override;
    void handleSms(common::PhoneNumber from, std::string text) override;
    
    void acceptDialing(common::PhoneNumber recipientPhoneNumber);
    void rejectDialing();
    void handleCallAccepted(common::PhoneNumber from) override;
    void handleCallDropped(common::PhoneNumber from) override;
    void acceptCallRequest() override;
    void rejectCallRequest() override;
    
private:
    common::PhoneNumber recipientPhoneNumber;
    common::PhoneNumber callingPhoneNumber;
    bool callRequestSent = false;
    bool receivingCallRequest = false;
    static const std::chrono::seconds CALL_REQUEST_TIMEOUT;
    static const std::chrono::seconds CALL_RESPONSE_TIMEOUT;
};

}