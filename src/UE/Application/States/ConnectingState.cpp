#include "ConnectingState.hpp"
#include "ConnectedState.hpp"
#include "NotConnectedState.hpp"

namespace ue
{

ConnectingState::ConnectingState(Context &context, common::BtsId btsId)
    : BaseState(context, "ConnectingState"),
      btsId(btsId)
{
    logger.logInfo("Trying to attach to BTS: ", btsId);
    context.bts.sendAttachRequest(btsId);
    context.timer.startTimer(std::chrono::milliseconds{500});
}

void ConnectingState::handleTimeout()
{
    logger.logError("Attachment timeout!");
    context.setState<NotConnectedState>();
}

void ConnectingState::handleAttachAccept()
{
    logger.logInfo("Attached to BTS successfully");
    context.timer.stopTimer();
    context.setState<ConnectedState>();
}

void ConnectingState::handleAttachReject()
{
    logger.logError("Attachment rejected!");
    context.timer.stopTimer();
    context.setState<NotConnectedState>();
}

}