#include "TalkingState.h"
#include "NotConnectedState.hpp"

namespace ue {

TalkingState::TalkingState(Context& context, common::PhoneNumber to)
    : BaseState(context, "TalkingState"),
      to(to)
{
    context.user.showTalking();
}

void TalkingState::handleDisconnect()
{
    logger.logInfo("Disconnected during call");
    context.setState<NotConnectedState>();
}

void TalkingState::sendTalkMessage(const std::string& text)
{
    context.bts.sendTalkMessage(common::PhoneNumber to, std::string text);
    context.timer.stopTimer();
    context.timer.startTimer(std::chrono::minutes(2));
}

}
