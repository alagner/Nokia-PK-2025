#include "ReceivingCallState.hpp"
#include "ConnectedState.hpp"
#include "UeGui/IDialMode.hpp"
#include <chrono>

namespace ue
{

ReceivingCallState::ReceivingCallState(Context& context, common::PhoneNumber caller)
  : BaseState(context, "ReceivingCallState"), caller(caller)
{
    context.user.showIncomingCall(caller);
    context.timer.startTimer(std::chrono::seconds(30));
}

void ReceivingCallState::handleUserAction(const std::string& id)
{
    logger.logDebug("ReceivingCallState handleUserAction: ", id);
    if(id == "ACCEPT") {
        context.timer.stopTimer();
        context.bts.sendCallAccepted(caller);
        logger.logInfo("User accepted call from ", caller, ". Transitioning to ConnectedState.");
        context.setState<ConnectedState>(); // Back to ConnectedState (todo talking)
    }
    else if(id == "REJECT") {
        context.timer.stopTimer();
        context.bts.sendCallDropped(caller);
        logger.logInfo("User rejected call from ", caller, ". Transitioning to ConnectedState.");
        context.setState<ConnectedState>();
    }
    else {
        BaseState::handleUserAction(id);
    }
}

void ReceivingCallState::handleTimeout() {
    logger.logInfo("ReceivingCallState timeout occurred. Treating as rejection.");
    context.timer.stopTimer();
    context.bts.sendCallDropped(caller);
    context.setState<ConnectedState>();
}

void ReceivingCallState::handleCallAccepted(common::PhoneNumber from) {
    logger.logError("Unexpected CallAccepted in ReceivingCallState");
}

void ReceivingCallState::handleCallDropped(common::PhoneNumber from) {
    context.timer.stopTimer();
    logger.logInfo("Received CallDropped in ReceivingCallState. Transitioning to ConnectedState.");
    context.setState<ConnectedState>();
}

void ReceivingCallState::handleCallTalk(common::PhoneNumber from, const std::string& text) {
    logger.logError("Unexpected CallTalk in ReceivingCallState");
}

void ReceivingCallState::handleDisconnect()
{
    logger.logInfo("ReceivingCallState: transport lost.");
    context.timer.stopTimer();
    context.setState<NotConnectedState>();
}

void ReceivingCallState::handleUnknownRecipient(common::MessageId msgId, common::PhoneNumber from)
{
    logger.logInfo("ReceivingCallState: UnknownRecipient received. Transitioning to ConnectedState.");
    context.timer.stopTimer();
    context.setState<ConnectedState>();
}

} // namespace ue
