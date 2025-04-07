#include "Application.hpp"
#include "States/NotConnectedState.hpp"

namespace ue {

    Application::Application(common::PhoneNumber phoneNumber,
                             common::ILogger &iLogger, IBtsPort &bts,
                             IUserPort &user, ITimerPort &timer)
        : context{iLogger, bts, user, timer}, logger(iLogger, "[APP] ") {
        logger.logInfo("Started");
        context.setState<NotConnectedState>();
    }

    Application::~Application() { logger.logInfo("Stopped"); }

    void Application::handleTimeout() { context.state->handleTimeout(); }

    void Application::handleSib(common::BtsId btsId) {
        context.state->handleSib(btsId);
    }

    void Application::handleAttachAccept() { context.state->handleAttachAccept(); }

    void Application::handleAttachReject() { context.state->handleAttachReject(); }

    void Application::handleDisconnected()
    {
        logger.logInfo("Transport disconnected");
        context.state->handleDisconnected();
    }

    void Application::handleSmsReceived(common::PhoneNumber fromNumber, std::string message)
    {
        logger.logInfo("SMS received from: ", fromNumber);
        context.state->handleSmsReceived(fromNumber, message);
    }

}
