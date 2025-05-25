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
    void handleCallRecipientNotAvailable() override;
    void handleCallDropped() override;
    void handleCallRequest(common::PhoneNumber from) override;
    void callAccept(common::PhoneNumber from) override;
    void callDrop() override;


    // IUserEventsHandler interface
    void viewSmsList() override;
    void viewSms(unsigned int) override;
    void sendSms(const SmsEntity& sms) override;
    void composeSms() override;
    void startDial() override;
    void sendCallRequest(common::PhoneNumber number) override;
    void handleCallAccepted(common::PhoneNumber from) override;
    void cancelCallRequest() override;
    void handleTimeout() override;
    void handleRedirect() override;
    void handleTalkMessage(common::PhoneNumber from, const std::string& text) override;

   private:
    common::PhoneNumber callTarget{};
    common::PhoneNumber callSender{};
};

}