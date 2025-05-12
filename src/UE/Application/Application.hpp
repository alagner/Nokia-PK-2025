#pragma once

#include "Logger/PrefixedLogger.hpp"
#include "Messages/PhoneNumber.hpp"
#include "IEventsHandler.hpp"
#include "Context.hpp"
#include "Ports/IBtsPort.hpp"
#include "Ports/IUserPort.hpp"
#include "Ports/ITimerPort.hpp"

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

    // ITimerEventsHandler interface
    void handleTimeout() override;

    // IBtsEventsHandler interface
    void handleSib(common::BtsId btsId) override;
    void handleAttachAccept() override;
    void handleAttachReject() override;
    void handleDisconnected() override;

    // ui related
    void handleUiAction(std::optional<std::size_t> ind) override;
    void handleUiBack() override;

    // Sms interface
    void handleSmsReceive(common::PhoneNumber from, std::string messageText) override;
    void handleSmsSent(common::PhoneNumber to, bool succeed) override;
    void handleSmsCompose(common::PhoneNumber to, const std::string& textMessage) override;

private:
    Context context;
    common::PrefixedLogger logger;
};

}
