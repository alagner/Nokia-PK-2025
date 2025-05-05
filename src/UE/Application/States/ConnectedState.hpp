#pragma once

#include "BaseState.hpp"

namespace ue
{

class ConnectedState : public BaseState
{
public:
    ConnectedState(Context& context);
    void handleDisconnect() override;
    void handleSms(common::PhoneNumber from, std::string text) override;
    void handleCallRequest(common::PhoneNumber from) override;
    void handleTimeout() override;
    
    void acceptCallRequest() override;
    void rejectCallRequest() override;
    
    // Override the dial method from BaseState (fixing the method name)
    void dial() override;
    
private:
    common::PhoneNumber callingPhoneNumber;
    static const std::chrono::seconds CALL_TIMEOUT;
};

}
