#pragma once

#include "IEventsHandler.hpp"
#include "Logger/PrefixedLogger.hpp"
#include "Context.hpp"
#include "Messages/PhoneNumber.hpp" // Include for PhoneNumber
#include <string>                  // Include for string
#include "Messages/MessageId.hpp"

namespace ue
{

class BaseState : public IEventsHandler
{
public:
    BaseState(Context& context, const std::string& name);
    ~BaseState() override;

    // ITimerEventsHandler interface
    void handleTimeout() override;

    // IBtsEventsHandler interface
    void handleSib(common::BtsId btsId) override;
    void handleAttachAccept() override;
    void handleAttachReject() override;
    void handleDisconnect() override;
    void handleSms(const common::PhoneNumber& from, const std::string& text) override;

    // IUserEventsHandler interface
    virtual void handleUserAction(const std::string& id) override;

    virtual void handleCallRequest(common::PhoneNumber from) override;
    virtual void handleCallAccepted(common::PhoneNumber from) override;
    virtual void handleCallDropped(common::PhoneNumber from) override;
    virtual void handleCallTalk(common::PhoneNumber from, const std::string& text) override;
    virtual void handleUnknownRecipient(common::MessageId, common::PhoneNumber from) override;

protected:
    Context& context;
    common::PrefixedLogger logger;
};

}