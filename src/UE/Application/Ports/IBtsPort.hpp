#pragma once

#include "Messages/BtsId.hpp"
#include "Messages/PhoneNumber.hpp"
#include "Messages/MessageId.hpp"
#include <string>

namespace ue
{

class IBtsEventsHandler
{
public:
    virtual ~IBtsEventsHandler() = default;

    virtual void handleSib(common::BtsId) = 0;
    virtual void handleAttachAccept() = 0;
    virtual void handleAttachReject() = 0;
    virtual void handleDisconnect() = 0;
    virtual void handleSms(const common::PhoneNumber& from, const std::string& text) = 0;
    // Add handleUnknownRecipient later if needed
    virtual void handleCallRequest(common::PhoneNumber from) = 0;
    virtual void handleCallAccepted(common::PhoneNumber from) = 0;
    virtual void handleCallDropped(common::PhoneNumber from) = 0;
    virtual void handleCallTalk(common::PhoneNumber from, const std::string& text) = 0;

    virtual void handleUnknownRecipient(common::MessageId originalMessageId, common::PhoneNumber from) = 0;
};

class IBtsPort
{
public:
    virtual ~IBtsPort() = default;

    virtual void sendAttachRequest(common::BtsId) = 0;
    // Added for Sending SMS
    virtual void sendSms(const common::PhoneNumber& recipient, const std::string& text) = 0;

    
    virtual void sendCallRequest(const common::PhoneNumber& recipient) = 0;
    virtual void sendCallAccepted(const common::PhoneNumber& recipient) = 0;
    virtual void sendCallDropped(const common::PhoneNumber& recipient) = 0;
    virtual void sendCallTalk(const common::PhoneNumber& recipient, const std::string& text) = 0;

};

} // namespace ue