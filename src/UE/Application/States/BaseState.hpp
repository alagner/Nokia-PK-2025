#pragma once

#include "IEventsHandler.hpp"
#include "Logger/PrefixedLogger.hpp"
#include "Context.hpp"
#include "Messages/PhoneNumber.hpp" // Include for PhoneNumber
#include <string>                  // Include for string

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
    // CORRECTED: Added virtual handleUserAction override
    virtual void handleUserAction(const std::string& id) override;

protected:
    Context& context;
    common::PrefixedLogger logger;
};

} // namespace ue