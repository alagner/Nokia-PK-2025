#pragma once

#include "IEventsHandler.hpp"
#include "Logger/PrefixedLogger.hpp"
#include "Context.hpp"

namespace ue
{

class BaseState : public IEventsHandler
{
public:
    BaseState(Context& context, const std::string& name);
    ~BaseState() override;

    // IUserEventsHandler interface
    void viewSmsList() override;
    void viewSms(unsigned int) override;
    void sendSms(const SmsEntity& sms) override;
    void composeSms() override;
    void startDial() override;
    void sendCallRequest(common::PhoneNumber number) override;
    void handleCallAccepted(common::PhoneNumber from) override;
    void cancelCallRequest() override;
    void handleRedirect() override;
    void sendTalkMessage(const std::string& text) override;
    void callAccept(common::PhoneNumber from) override;
    void callDrop() override;

    // ITimerEventsHandler interface
    void handleTimeout() override;

    // IBtsEventsHandler interface
    void handleSib(common::BtsId btsId) override;
    void handleAttachAccept() override;
    void handleAttachReject() override;
    void handleDisconnect() override;
    void handleSms(common::PhoneNumber from, std::string text);
    void handleSmsDeliveryFailure(common::PhoneNumber);
    void handleCallRecipientNotAvailable() override;
    void handleCallDropped() override;
    void handleCallRequest(common::PhoneNumber) override;

protected:
    Context& context;
    common::PrefixedLogger logger;
};

}