#include "TalkingState.hpp"
#include "ConnectedState.hpp"

namespace ue
{

TalkingState::TalkingState(Context &context, common::PhoneNumber phoneNumber)
    : BaseState(context, "TalkingState"),
      peerPhoneNumber(phoneNumber)
{
    context.user.showCallView(phoneNumber);
}

void TalkingState::handleDisconnect()
{
    context.setState<ConnectedState>();
}

void TalkingState::handleCallTalk(common::PhoneNumber from, std::string text)
{
    if (from == peerPhoneNumber) {
        logger.logInfo("Received call message from: ", from, ", text: ", text);
        context.user.showCallMessage(text);
    } else {
        logger.logError("Received call message from unknown peer: ", from, ", expected: ", peerPhoneNumber);
    }
}

void TalkingState::handleCallDropped(common::PhoneNumber from)
{
    if (from == peerPhoneNumber) {
        logger.logInfo("Call dropped by peer: ", from);
        context.user.showPeerUserNotAvailable(from);
        logger.logInfo("Returning to ConnectedState");
        context.setState<ConnectedState>();
    } else {
        logger.logError("Received call dropped from unknown peer: ", from, ", expected: ", peerPhoneNumber);
    }
}

}