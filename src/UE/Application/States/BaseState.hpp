#pragma once

#include "IEventsHandler.hpp"
#include "Logger/PrefixedLogger.hpp"
#include "Context.hpp"

namespace ue{

class BaseState : public IEventsHandler{
public:
    BaseState(Context& context, const std::string& name);
    ~BaseState() override;

    void handleTimeout() override;

    void handleSib(common::BtsId btsId) override;
    void handleAttachAccept() override;
    void handleAttachReject() override;
    void handleDisconnected() override;

    void handleMessageReceive(common::PhoneNumber sender, std::string text) override;
    void handleMessageSentResult(common::PhoneNumber to, bool success) override;
    void handleMessageComposeResult(common::PhoneNumber reciver, const std::string& text) override;
    void handleUiAction(std::optional<std::size_t> selectedIndex) override;
    void handleUiBack() override;

protected:
    Context& context;
    common::PrefixedLogger logger;
};

}
