#pragma once

#include "BaseState.hpp"
#include "Messages/PhoneNumber.hpp"

namespace ue
{

class ConnectedState : public BaseState
{
public:
    ConnectedState(Context& context);

    // IBtsEventsHandler interface
    void handleDisconnect() override;
    void handleSms(common::PhoneNumber from, std::string text) override;
    void handleSmsDeliveryFailure(common::PhoneNumber from) override;

    // IUserEventsHandler interface
    void viewSmsList() override;
    void viewSms(unsigned int) override;
    void sendSms(const SmsEntity& sms) override;
    void composeSms() override;
};

}
