#pragma once

#include "Messages/BtsId.hpp"
#include "Messages/PhoneNumber.hpp"
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
};

class IBtsPort
{
public:
    virtual ~IBtsPort() = default;

    virtual void sendAttachRequest(common::BtsId) = 0;
    // Added for Sending SMS
    virtual void sendSms(const common::PhoneNumber& recipient, const std::string& text) = 0;
};

} // namespace ue