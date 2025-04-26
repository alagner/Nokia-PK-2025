#include "TalkingState.hpp"
#include "ConnectedState.hpp"
#include "NotConnectedState.hpp"

namespace ue
{

    TalkingState::TalkingState(Context &context, common::PhoneNumber peerNumber)
        : BaseState(context, "TalkingState"),
          peerPhoneNumber(peerNumber)
    {
        logger.logInfo("Entered talking state with: ", peerPhoneNumber);
        context.user.showTalkingScreen(peerPhoneNumber);
    }

    void TalkingState::handleUiBack()
    {
        logger.logInfo("User hangs up call with: ", peerPhoneNumber);
        context.bts.sendCallDropped(peerPhoneNumber);
        context.setState<ConnectedState>();
    }

    void TalkingState::handleCallDropped(common::PhoneNumber peer)
    {
        if (peer == peerPhoneNumber)
        {
            logger.logInfo("Call dropped by peer: ", peer);
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

    void TalkingState::handleUiAction(std::optional<std::size_t>)
    {
        std::string msg = context.user.getCallText();
        if (msg.empty())
        {
            logger.logInfo("Empty message in talking screen – ignored");
            return;
        }

        context.bts.sendCallTalk(peerPhoneNumber, msg);
        context.user.clearOutgoingText();
    }

    void TalkingState::handleCallTalk(common::PhoneNumber from, const std::string &text)
    {
        if (from != peerPhoneNumber)
        {
            logger.logError("Unexpected CallTalk from ", from);
            return;
        }

        context.user.appendIncomingText(text);
    }

}