#pragma once

#include "Logger/PrefixedLogger.hpp"
#include "Messages/PhoneNumber.hpp"
#include "IEventsHandler.hpp"
#include "Context.hpp"
#include "SmsRepository/SmsRepository.h"

namespace ue
{

using common::PhoneNumber;
using common::ILogger;

class Application : public IEventsHandler
{
public:
    Application(PhoneNumber phoneNumber,
                ILogger& iLogger,
                IBtsPort& bts,
                IUserPort& user,
                ITimerPort& timer,
                ISmsRepository& SmsDb);
    ~Application();

    // IUserEventsHandler interface
    void viewSmsList() override;
    void viewSms(unsigned int) override;
    void sendSms(const SmsEntity& sms) override;
    void composeSms() override;
    void startDial() override;
    void sendCallRequest(common::PhoneNumber number) override;
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
    void handleSms(common::PhoneNumber from, std::string text) override;
    void handleSmsDeliveryFailure(common::PhoneNumber from) override;
    void handleCallAccepted(PhoneNumber from) override;
    void handleCallDropped() override;
    void handleCallRecipientNotAvailable() override;
    void handleTalkMessage(common::PhoneNumber from, const std::string& text);
    void handleCallRequest(common::PhoneNumber from) override;

private:
    Context context;
    common::PrefixedLogger logger;
    common::PhoneNumber phoneNumber;
};

}
