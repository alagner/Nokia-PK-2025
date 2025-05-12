#pragma once

#include "IEventsHandler.hpp"
#include "Logger/PrefixedLogger.hpp"
#include "Context.hpp"
#include "Messages/MessageId.hpp"
namespace ue
{

class BaseState : public IEventsHandler
{
public:
    BaseState(Context& context, const std::string& name);
    ~BaseState() override;

    // Timer events  -  ITimerEventsHandler
    void handleTimeout() override;

    // BTS events  -  IBtsEventsHandler
    void handleSib(common::BtsId btsId) override;
    void handleAttachAccept() override;
    void handleAttachReject() override;
    void handleDisconnected() override;
    void handleSmsReceive(common::PhoneNumber from, std::string messageText) override;

    // User-triggered events
    void handleUiAction(std::optional<std::size_t> ind) override;
    void handleUiBack() override;
    void handleSmsSent(common::PhoneNumber to, bool succeed) override;
    void handleSmsCompose(common::PhoneNumber to, const std::string& textMessage) override;

protected:
    Context& context;
    common::PrefixedLogger logger;
};

}
