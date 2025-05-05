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
};

class IUserPort
{
public:
    virtual ~IUserPort() = default;

    virtual void showNotConnected() = 0;
    virtual void showConnecting() = 0;
    virtual void showConnected() = 0;
    virtual void showNewSms(bool present) = 0;  // Fixed to include boolean parameter
    
    // Call-related UI methods
    virtual void showCallRequest(common::PhoneNumber phoneNumber) = 0;
    virtual void showCallView(common::PhoneNumber phoneNumber) = 0;
    virtual void showCallMessage(std::string message) = 0;
    virtual void showPeerUserNotAvailable(common::PhoneNumber phoneNumber) = 0;
    
    virtual void showDialView() = 0;
};

}
