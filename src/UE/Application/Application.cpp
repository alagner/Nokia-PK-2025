#include "Application.hpp"
#include "States/NotConnectedState.hpp"
#include "States/ConnectedState.hpp"

namespace ue
{

    Application::Application(common::PhoneNumber phoneNumber,
                             common::ILogger &iLogger,
                             IBtsPort &bts,
                             IUserPort &user,
                             ITimerPort &timer)
        : context{iLogger, bts, user, timer, SmsDb(), nullptr, phoneNumber},
          logger(iLogger, "[APP] ")
    {
        logger.logInfo("Started with phone number: ", context.myPhoneNumber);
        context.setState<NotConnectedState>();
    }

    Application::~Application()
    {
        logger.logInfo("Stopped");
    }

    void Application::handleUiAction(std::optional<std::size_t> selectedIndex)
    {
        if (context.state)
            context.state->handleUiAction(selectedIndex);
    }

    void Application::handleUiBack()
    {
        if (context.state)
            context.state->handleUiBack();
    }

    void Application::handleTimeout()
    {
        if (context.state)
            context.state->handleTimeout();
    }

    void Application::handleSib(common::BtsId btsId)
    {
        if (context.state)
            context.state->handleSib(btsId);
    }

    void Application::handleAttachAccept()
    {
        if (context.state)
            context.state->handleAttachAccept();
    }

    void Application::handleAttachReject()
    {
        if (context.state)
            context.state->handleAttachReject();
    }

    void Application::handleDisconnected()
    {
        logger.logInfo("Transport disconnected");
        if (context.state)
            context.state->handleDisconnected();
    }

    void Application::handleSmsReceived(common::PhoneNumber from, std::string text)
    {
        logger.logInfo("SMS received from: ", from);
        if (context.state)
            context.state->handleSmsReceived(from, text);
    }

    void Application::handleSmsSentResult(common::PhoneNumber to, bool success)
    {
        logger.logInfo("Handling SMS send result for: ", to, ", Success: ", success);
        if (context.state)
            context.state->handleSmsSentResult(to, success);
    }

    void Application::handleSmsComposeResult(common::PhoneNumber recipient, const std::string &text)
    {
        context.smsDb.addSentSms(recipient, text);
        context.bts.sendSms(recipient, text);

        context.setState<ConnectedState>();
    }

}
