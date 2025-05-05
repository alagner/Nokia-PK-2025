#pragma once

#include "BaseState.hpp"

namespace ue
{

class DialState : public BaseState
{
public:
    DialState(Context& context);
    void handleTimeout() override;
    
    void acceptDialing(common::PhoneNumber recipientPhoneNumber);
    void rejectDialing();
    void handleCallAccepted(common::PhoneNumber from) override;
    void handleCallDropped(common::PhoneNumber from) override;
    
private:
    common::PhoneNumber recipientPhoneNumber;
    static const std::chrono::seconds CALL_REQUEST_TIMEOUT;
};

}