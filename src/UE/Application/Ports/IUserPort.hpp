#pragma once

#include <vector>
#include <string>
#include "SmsStorage.hpp"

namespace ue{

class IUserEventsHandler{
public:
    virtual ~IUserEventsHandler() = default;
};

class IUserPort{
public:
    virtual ~IUserPort() = default;

    virtual void showNotConnected() = 0;
    virtual void showConnecting() = 0;
    virtual void showConnected() = 0;
    virtual void showNewMessage() = 0;

    virtual void showListMessage(const std:: vector<SmsMessage>& messages) = 0;
    virtual void showMessageView(const SmsMessage& message) = 0;
    virtual void showNotify(const std::string& name, const std::string& message) = 0;
    virtual void showMessageComp() = 0;

    virtual common::PhoneNumber getMessageRecipient() const = 0;
    virtual std::string getMessageText() const = 0;
};

}
