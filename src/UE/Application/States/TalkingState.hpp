#pragma once

#include "BaseState.hpp"

namespace ue
{

class TalkingState : public BaseState
{
public:
    TalkingState(Context& context, common::PhoneNumber phoneNumber);
    void handleDisconnect() override;
    void handleCallTalk(common::PhoneNumber from, std::string text);
    void handleCallDropped(common::PhoneNumber from);
    void rejectDialing() override; 
    
private:
    common::PhoneNumber peerPhoneNumber;
};

}