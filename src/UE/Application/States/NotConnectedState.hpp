#pragma once

#include "BaseState.hpp"

namespace ue
{

class NotConnectedState : public BaseState
{
public:
    NotConnectedState(Context& context);
    // IBtsEventsHandler interface
public:
    void handleSib(common::BtsId btsId) override;
    void sendSms(const SmsEntity& sms) override;
    void composeSms() override;
    void handleTalkMessage(common::PhoneNumber from, const std::string& text) override;
};

}
