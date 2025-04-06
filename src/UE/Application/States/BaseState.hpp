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

    void handleTimeout() override;

    void handleSib(common::BtsId btsId) override;
    void handleAttachAccept() override;
    void handleAttachReject() override;
    void handleDisconnected() override;
    void handleSmsReceived(common::PhoneNumber from, std::string text) override;
    void handleSmsSentResult(common::PhoneNumber to, bool success) override;
    void handleSmsComposeResult(common::PhoneNumber recipient, const std::string& text) override;
    void handleUiAction(std::optional<std::size_t> selectedIndex) override;
    void handleCallRequest(common::PhoneNumber from) override;
    void handleCallEnd(common::PhoneNumber peer) override;
    void handleUiBack() override;
protected:
    Context& context;
    common::PrefixedLogger logger;
};

}
