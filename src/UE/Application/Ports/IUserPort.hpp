#pragma once

#include <vector> 
#include <string>
#include "SmsDb.hpp"

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
    virtual void showNewSms() = 0;

    virtual void showSmsList(const std::vector<SmsMessage>& messages) = 0;
    virtual void showSmsView(const SmsMessage& message) = 0;
    virtual void showAlert(const std::string& title, const std::string& message) = 0;
};

}
