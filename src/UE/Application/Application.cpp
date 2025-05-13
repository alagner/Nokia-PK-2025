#include "Application.hpp"
#include "States/NotConnectedState.hpp"
#include "States/ConnectedState.hpp"

namespace ue{

Application::Application(common::PhoneNumber phoneNumber,
                         common::ILogger &iLogger,
                         IBtsPort &bts,
                         IUserPort &user,
                         ITimerPort &timer)
    : context{iLogger, bts, user, timer},
      logger(iLogger, "[APP] "){
    logger.logInfo("Started");
    context.setState<NotConnectedState>();
}

Application::~Application(){
    logger.logInfo("Stopped");
}

void Application::handleUiAction(std::optional<std::size_t> selectedIndex){
    if (context.state)
        context.state->handleUiAction(selectedIndex);
}

void Application::handleUiBack(){
    if (context.state)
        context.state->handleUiBack();
}

void Application::handleTimeout(){
    context.state->handleTimeout();
}

void Application::handleSib(common::BtsId btsId){
    context.state->handleSib(btsId);
}

void Application::handleAttachAccept(){
    context.state->handleAttachAccept();
}

void Application::handleAttachReject(){
    context.state->handleAttachReject();
}

void Application::handleDisconnected(){
    context.state->handleDisconnected();
}

void Application::handleCallRequest(common::PhoneNumber from) {
   logger.logInfo("Incoming Call");
   context.state->handleCallRequest(from);
}

void Application::handleMessageReceive( common::PhoneNumber sender, std::string text){
    logger.logInfo("SMS received, sender: ", sender);
    context.state->handleMessageReceive(sender, text);
}

void Application::handleMessageSentResult(common::PhoneNumber to, bool success){
    logger.logInfo("Handling SMS send result for: ", to, ", Success: ", success);
    if (context.state)
        context.state->handleMessageSentResult(to, success);
}

void Application::handleMessageComposeResult(common::PhoneNumber reciver, const std::string &text){
    context.smsStorage.addMessage(reciver, text);
    context.bts.sendMessage(reciver, text);

    context.setState<ConnectedState>();
}
}
