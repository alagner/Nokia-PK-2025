#pragma once

#include <Models/SmsDatabase.hpp>

namespace ue
{

class IUserEventsHandler
{
public:
    virtual ~IUserEventsHandler() = default;
};

class IUserPort
{
public:
    virtual ~IUserPort() = default;

    virtual void showNotConnected() = 0;
    virtual void showConnecting() = 0;
    virtual void showConnected() = 0;
    virtual void showSms() = 0;
    virtual void showSmsList(const std::vector<SmsMessage>& textMessages) = 0;
    virtual void showSmsView(const SmsMessage& textMessage) = 0;
    virtual void showAlert(const std::string& title, const std::string& textMessage) = 0;
    virtual void showSmsCompose() = 0;
    virtual common::PhoneNumber getSmsRecipient() = 0;
    virtual std::string getSmsTextMessage() = 0;

    //TODO: here
};

}
