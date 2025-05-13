#include "TalkingState.hpp"
#include "ConnectedState.hpp"
#include "NotConnectedState.hpp"
#include <chrono>

using namespace std::chrono;

namespace ue
{

    TalkingState::TalkingState(Context &context, common::PhoneNumber peerNumber)
        : BaseState(context, "TalkingState"),
          peerPhoneNumber(peerNumber)
    {
        logger.logInfo("Entered talking state with: ", peerPhoneNumber);
        context.user.showTalkingScreen(peerPhoneNumber);
        context.timer.startTimer(minutes(2));
    }

    void TalkingState::handleUiBack()
    {
        logger.logInfo("User hangs up call with: ", peerPhoneNumber);
        context.timer.stopTimer();
        context.bts.sendCallDropped(peerPhoneNumber);
        context.user.showAlert("Call Ended", "You ended the call.");
        awaitingUserAfterCallEnd = true;
    }

    void TalkingState::handleCallDropped(common::PhoneNumber peer)
    {
        if (peer == peerPhoneNumber)
        {
            logger.logInfo("Call dropped by peer: ", peer);
            context.timer.stopTimer();
            context.user.showAlert("Call Ended", "Peer has ended the call.");
            awaitingUserAfterCallEnd = true;
        }
    }

    void TalkingState::handleDisconnected()
    {
        logger.logError("Disconnected during call with: ", peerPhoneNumber);
        context.timer.stopTimer();
        context.setState<NotConnectedState>();
    }

    void TalkingState::handleUiAction(std::optional<std::size_t>)
    {
        if (awaitingUserAfterCallEnd)
        {
            logger.logInfo("User acknowledged call end alert");
            context.setState<ConnectedState>();
            return;
        }

        std::string msg = context.user.getCallText();
        if (msg.empty())
        {
            logger.logInfo("Empty message in talking screen – ignored");
            return;
        }

        context.bts.sendCallTalk(peerPhoneNumber, msg);
        context.user.clearOutgoingText();
        context.timer.startTimer(minutes(2));
    }

    void TalkingState::handleCallTalk(common::PhoneNumber from, const std::string &text)
    {
        if (from != peerPhoneNumber)
        {
            logger.logError("Unexpected CallTalk from ", from);
            return;
        }

        context.user.appendIncomingText(text);
        context.timer.startTimer(minutes(2));
    }

    void TalkingState::handleTimeout()
    {
        logger.logInfo("No activity for 2 minutes, ending call.");
        context.bts.sendCallDropped(peerPhoneNumber);
        context.user.showAlert("Call Ended", "No activity for 2 minutes.");
        context.setState<ConnectedState>();
    }

    void TalkingState::handleUnknownRecipient(common::PhoneNumber peer)
    {
        if (peer == peerPhoneNumber)
        {
            logger.logInfo("Peer disconnected - UnknownRecipient");
            context.timer.stopTimer();
            context.user.showAlert("Call Failed", "Peer UE is no longer connected.");
            context.setState<ConnectedState>();
        }
        else
        {
            logger.logError("UnknownRecipient from unexpected peer: ", peer);
        }
    }

    void TalkingState::handleSmsReceived(common::PhoneNumber from, std::string text)
    {
        logger.logInfo("SMS received during call from: ", from);
        context.smsDb.addReceivedSms(from, text);
        context.user.showNewSms();
    }

}