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
    : context{iLogger, bts, user, timer},
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

}
