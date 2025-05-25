#include "TalkingState.h"
#include "NotConnectedState.hpp"
#include "ConnectedState.hpp"

namespace ue {

TalkingState::TalkingState(Context& context, common::PhoneNumber to)
    : BaseState(context, "TalkingState"),
      to(to)
{
    context.user.showTalking(to);
}

void TalkingState::handleDisconnect()
{
    logger.logInfo("Disconnected during call");
    context.setState<NotConnectedState>();
}

void TalkingState::handleSms(common::PhoneNumber from, std::string text)
{
    logger.logInfo(common::ILogger::INFO_LEVEL, "[ConnectedState] Received SMS from: ", from, ", text: ", text);
    context.user.showNewSms();
}

void TalkingState::sendTalkMessage(const std::string& text)
{
    context.bts.sendTalkMessage(to, text);
    context.timer.stopTimer();
    context.timer.startTimer(std::chrono::minutes(2));
}

void TalkingState::handleTalkMessage(common::PhoneNumber from, const std::string& text)
{
    context.user.displayMessage(from, text);
    context.timer.stopTimer();
    context.timer.startTimer(std::chrono::minutes(2));
}

void TalkingState::handleTimeout()
{
    logger.logInfo("Timeout: no activity, dropping call");
    context.bts.sendCallDropped(context.user.getPhoneNumber(), to);
    context.user.showConnected();
    context.setState<ConnectedState>();
}

void TalkingState::handleCallRecipientNotAvailable()
{
    logger.logInfo("Peer is unavailable during call");
    context.user.showPartnerNotAvailable(to);
    context.timer.stopTimer();
    context.timer.startRedirectTimer(std::chrono::seconds(3));
}

void TalkingState::handleRedirect()
{
    context.timer.stopTimer();
    context.user.showConnected();
    context.setState<ConnectedState>();
}

void TalkingState::callDrop()
{
    logger.logInfo("User dropped the call");
    context.timer.stopTimer();
    context.bts.sendCallDropped(context.user.getPhoneNumber(), to);
    context.user.showConnected();
    context.setState<ConnectedState>();
}

void TalkingState::handleCallDropped()
{
    logger.logInfo("Peer dropped the call");
    context.timer.stopTimer();
    context.user.showConnected();
    context.setState<ConnectedState>();
}

void TalkingState::handleCallRequest(common::PhoneNumber from)
{
    context.timer.stopTimer();
    context.bts.sendCallDropped(context.user.getPhoneNumber(), from);
}

}
