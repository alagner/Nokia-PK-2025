#pragma once

#include "Logger/PrefixedLogger.hpp"
#include "Messages/PhoneNumber.hpp"
#include "IEventsHandler.hpp"
#include "Context.hpp"
#include "Ports/IBtsPort.hpp"
#include "Ports/IUserPort.hpp"
#include "Ports/ITimerPort.hpp"
#include "SmsDb.hpp"

namespace ue
{

    using common::ILogger;
    using common::PhoneNumber;

    class Application : public IEventsHandler
    {
    public:
        Application(PhoneNumber phoneNumber,
                    ILogger &iLogger,
                    IBtsPort &bts,
                    IUserPort &user,
                    ITimerPort &timer);
        ~Application();

        void handleUiAction(std::optional<std::size_t> selectedIndex) override;
        void handleUiBack() override;
        void handleTimeout() override;

        void handleSib(common::BtsId btsId) override;
        void handleAttachAccept() override;
        void handleAttachReject() override;
        void handleDisconnected() override;
        void handleSmsReceived(common::PhoneNumber from, std::string text) override;
        void handleSmsSentResult(common::PhoneNumber to, bool success) override;
        void handleSmsComposeResult(common::PhoneNumber recipient, const std::string &text) override;
        void handleCallRequest(common::PhoneNumber from) override;
        void handleCallDropped(common::PhoneNumber peer) override;
        void handleCallAccept(common::PhoneNumber peer) override;
        void handleUnknownRecipient(common::PhoneNumber peer) override;
        void handleCallTalk(common::PhoneNumber from, const std::string &text) override;
        void handleCallReject(common::PhoneNumber peer) override;

    private:
        Context context;
        common::PrefixedLogger logger;
    };

}
