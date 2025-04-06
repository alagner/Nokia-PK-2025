#include "TalkingState.hpp"
#include "ConnectedState.hpp"
#include "NotConnectedState.hpp"

namespace ue
{

TalkingState::TalkingState(Context& context, common::PhoneNumber peerNumber)
    : BaseState(context, "TalkingState"),
      peerPhoneNumber(peerNumber)
{
    logger.logInfo("Entered talking state with: ", peerPhoneNumber);
    context.user.showTalkingScreen(peerPhoneNumber);
}

void TalkingState::handleUiBack()
{
    logger.logInfo("User hangs up call with: ", peerPhoneNumber);
    context.bts.sendCallEnd(peerPhoneNumber);
    context.setState<ConnectedState>();
}

void TalkingState::handleCallEnd(common::PhoneNumber peer)
{
    if (peer == peerPhoneNumber)
    {
        logger.logInfo("Call ended by peer: ", peer);
        context.user.showConnected();
        context.setState<ConnectedState>();
    }
     else
    {
         logger.logError("Received CallEnd from unexpected peer ", peer, " while talking to ", peerPhoneNumber);
    }
}

void TalkingState::handleDisconnected()
{
    logger.logError("Disconnected during call with: ", peerPhoneNumber);
    context.setState<NotConnectedState>();
}

}