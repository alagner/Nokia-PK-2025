#pragma once

#include "Messages/BtsId.hpp"
#include "Messages/PhoneNumber.hpp"

namespace ue
{

class IBtsEventsHandler
{
public:
    virtual ~IBtsEventsHandler() = default;

    virtual void handleSib(common::BtsId) = 0;
    virtual void handleAttachAccept() = 0;
    virtual void handleAttachReject() = 0;
    virtual void handleDisconnected() = 0;
    virtual void handleSmsReceive(common::PhoneNumber from, std::string messageText) = 0;
    virtual void handleSmsSent(common::PhoneNumber to, bool succeed) = 0;
    virtual void handleCallRequest(common::PhoneNumber to) = 0;
    virtual void handleCallDropped(common::PhoneNumber to) = 0;
    virtual void handleAcceptCall(common::PhoneNumber to) = 0;
    virtual void handleNumberUnknown(common::PhoneNumber to) = 0;
    virtual void handleTalkCall(common::PhoneNumber to, const std::string &message) = 0;
    virtual void handleCallReject(common::PhoneNumber to) = 0;
};

class IBtsPort
{
public:
    virtual ~IBtsPort() = default;

    virtual void sendAttachRequest(common::BtsId) = 0;
    virtual void sendSms(common::PhoneNumber to, const std::string& textMessage) =0;
    virtual void sendCallRequest(common::PhoneNumber to) = 0;
    virtual void sendCallDropped(common::PhoneNumber to) = 0;
    virtual void callMissed(common::PhoneNumber to) = 0;
    virtual void sendAcceptCall(common::PhoneNumber to) = 0;
    virtual void sendRejectCall(common::PhoneNumber to) = 0;
    virtual void sendTalkCall(common::PhoneNumber to, const std::string &message) = 0;
};

}
