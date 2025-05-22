#pragma once

#include "Messages/PhoneNumber.hpp"
#include <string>

namespace ue
{

class IUserEventsHandler
{
public:
    virtual ~IUserEventsHandler() = default;
    
    virtual void acceptCallRequest() = 0;
    virtual void rejectCallRequest() = 0;
    
    virtual void dial() = 0;
    virtual void acceptDialing(common::PhoneNumber number) = 0;
    virtual void rejectDialing() = 0;
    
    virtual void viewSms() = 0;
    virtual void selectSms(size_t index) = 0;
    virtual void closeSmsView() = 0;
    
    virtual void composeSms() = 0;
    virtual void acceptSmsCompose(common::PhoneNumber number, const std::string& text) = 0;
    virtual void rejectSmsCompose() = 0;
};

class IUserPort
{
public:
    virtual ~IUserPort() = default;

    virtual void showNotConnected() = 0;
    virtual void showConnecting() = 0;
    virtual void showConnected() = 0;
    virtual void showNewSms(bool present) = 0;
    virtual void showSmsList() = 0;
    virtual void showSmsContent(const std::string& from, const std::string& text) = 0;
    virtual void showSentSmsContent(const std::string& to, const std::string& text) = 0;
    
    virtual void showCallRequest(common::PhoneNumber phoneNumber) = 0;
    virtual void showCallView(common::PhoneNumber phoneNumber) = 0;
    virtual void showCallMessage(std::string message) = 0;
    virtual void showPeerUserNotAvailable(common::PhoneNumber phoneNumber) = 0;
    
    virtual void showDialView() = 0;

    virtual void showSmsComposeView() = 0;

    virtual std::string getOutgoingCallText() = 0;
    virtual void clearOutgoingCallText() = 0;
    virtual void clearIncomingCallText() = 0;
};

}
