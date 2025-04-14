#pragma once

#include "Logger/PrefixedLogger.hpp"
#include "Messages/PhoneNumber.hpp"
#include "IEventsHandler.hpp"
#include "Context.hpp"
#include "SmsStorage.hpp"

namespace ue{

using common::PhoneNumber;
using common::ILogger;

class Application : public IEventsHandler{
public:
    Application(PhoneNumber phoneNumber,
                ILogger& iLogger,
                IBtsPort& bts,
                IUserPort& user,
                ITimerPort& timer);
    ~Application();

    void handleUiAction(std::optional<std::size_t> selectedIndex) override;
    void handleUiBack() override;

    void handleTimeout() override;

    void handleSib(common::BtsId btsId) override;
    void handleAttachAccept() override;
    void handleAttachReject() override;

    void handleDisconnected() override;

    void handleMessageReceive( common::PhoneNumber sender, std::string text) override;
    void handleMessageSentResult(common::PhoneNumber to, bool success) override;
    void handleMessageComposeResult(common::PhoneNumber reciver, const std::string& text) override;


private:
    Context context;
    common::PrefixedLogger logger;

};

}
