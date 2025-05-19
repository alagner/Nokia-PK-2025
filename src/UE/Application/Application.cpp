#include "Application.hpp"
#include "States/NotConnectedState.hpp"
#include "States/ConnectedState.hpp"

namespace ue {
    Application::Application(common::PhoneNumber phoneNumber,
                             common::ILogger &iLogger,
                             IBtsPort &bts,
                             IUserPort &user,
                             ITimerPort &timer)
        : context{iLogger, bts, user, timer, nullptr, SmsDatabase(), phoneNumber },
        logger(iLogger, "[APP] ")
    {
        logger.logInfo("Started");
        context.setState<NotConnectedState>();
    }

    Application::~Application()
    {
        logger.logInfo("Stopped");
    }// 500ms timeout as per requirements

    void Application::handleTimeout()
    {
        if (context.state)
            context.state->handleTimeout();
        else
            logger.logError("handleTimeout called with no active state!");
    }

    void Application::handleSib(common::BtsId btsId)
    {
        if (context.state)
            context.state->handleSib(btsId);
        else
            logger.logError("handleSib called with no active state!");
    }

    void Application::handleAttachAccept()
    {
        if (context.state)
            context.state->handleAttachAccept();
        else
            logger.logError("handleAttachAccept called with no active state!");
    }

    void Application::handleAttachReject()
    {
        if (context.state)
            context.state->handleAttachReject();
        else
            logger.logError("handleDisconnected called with no active state!");
    }

    void Application::handleDisconnected()
    {
        logger.logInfo("Transport disconnected");
        if (context.state)
            context.state->handleDisconnected();
        else
            logger.logError("handleDisconnected called with no active state!");
    }

    void Application::handleSmsReceive(common::PhoneNumber from, std::string messageText)
    {
        logger.logInfo("SMS received from number: ", from);

        if (context.state)
            context.state->handleSmsReceive(from,  messageText);
        else
            logger.logError("handleSmsReceive called with no active state!");
    }

    void Application::handleUiAction(std::optional<std::size_t> ind)
    {
        if (context.state)
            context.state->handleUiAction(ind);
        else
            logger.logError("handleUiAction called with no active state!");

    }

    void Application::handleSmsSent(common::PhoneNumber to, bool succeed)
    {
        logger.logInfo("Handling sms sending to: ", to, ", Success: ", succeed);
        if (context.state)
            context.state->handleSmsSent(to, succeed);
    }

    void Application::handleSmsCompose(common::PhoneNumber to, const std::string &textMessage)
    {
        context.smsDatabase.addOutgoingSms(to, textMessage);
        context.bts.sendSms(to, textMessage);

        context.setState<ConnectedState>();
    }

    void Application::handleUiBack()
    {
        if (context.state)
            context.state->handleUiBack();
        else
            logger.logError("handleUiBack called with no active state!");
    }

    void Application::handleCallRequest(common::PhoneNumber from)
    {
        logger.logInfo("Call request received from: ", from);

        if (context.state)
            context.state->handleCallRequest(from);
        else
            logger.logError("handleCallRequest called with no active state!");
    }

    void Application::handleDialRequest(common::PhoneNumber to)
    {
        logger.logDebug("Application: dial request to ", to);
        context.state->handleDialRequest(to);
    }

    void Application::handleCallDropped(common::PhoneNumber to)
    {
        logger.logInfo("Call dropped to: ", to);

        if (context.state)
            context.state->handleCallDropped(to);
        else
            logger.logError("handleCallDropped called with no active state!");
    }

    void Application::handleAcceptCall(common::PhoneNumber to)
    {
        logger.logInfo("Call accepted from: ", to);

        if (context.state)
            context.state->handleAcceptCall(to);
        else
            logger.logError("handleAcceptCall called with no active state!");
    }

    void Application::handleCallReject(common::PhoneNumber to)
    {
        logger.logInfo("Call rejected from: ", to);

        if (context.state)
            context.state->handleCallReject(to);
        else
            logger.logError("handleCallReject called with no active state!");
    }

    void Application::handleTalkCall(common::PhoneNumber to, const std::string &message)
    {
        logger.logInfo("Talk call from: ", to, " with message: ", message);

        if (context.state)
            context.state->handleTalkCall(to, message);
        else
            logger.logError("handleTalkCall called with no active state!");
    }

    void Application::handleNumberUnknown(common::PhoneNumber to)
    {
        logger.logInfo("Number unknown: ", to);

        if (context.state)
            context.state->handleNumberUnknown(to);
        else
            logger.logError("handleNumberUnknown called with no active state!");
    }
}