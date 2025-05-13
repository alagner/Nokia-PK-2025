#include "IncomingCallState.hpp"
#include "ConnectedState.hpp"
#include "TalkingState.hpp"
#include "NotConnectedState.hpp"

namespace ue
{

    IncomingCallState::IncomingCallState(Context &context, common::PhoneNumber callingNumber)
        : BaseState(context, "IncomingCallState"),
          callingPhoneNumber(callingNumber)
    {
        logger.logInfo("Incoming call from: ", callingPhoneNumber);
        context.user.showIncomingCall(callingPhoneNumber);
        context.timer.startTimer(CALL_TIMEOUT);
    }

    IncomingCallState::~IncomingCallState()
    {
        context.timer.stopTimer();
    }

    void IncomingCallState::handleUiAction(std::optional<std::size_t> selectedIndex)
    {
        logger.logInfo("Call accepted from: ", callingPhoneNumber);
        context.timer.stopTimer();
        context.bts.sendCallAccept(callingPhoneNumber);
        context.setState<TalkingState>(callingPhoneNumber);
    }

    void IncomingCallState::handleUiBack()
    {
        logger.logInfo("Call rejected");
        context.timer.stopTimer();
        context.bts.sendCallReject(callingPhoneNumber);
        context.setState<ConnectedState>();
    }

    void IncomingCallState::handleTimeout()
    {
        logger.logError("Incoming call timeout for: ", callingPhoneNumber, ". Rejecting.");
        context.timer.stopTimer();
        context.bts.sendCallDropped(callingPhoneNumber);
        context.setState<ConnectedState>();
    }

    void IncomingCallState::handleDisconnected()
    {
        logger.logError("Disconnected during incoming call from: ", callingPhoneNumber);
        context.timer.stopTimer();
        context.setState<NotConnectedState>();
    }

    void IncomingCallState::handleCallDropped(common::PhoneNumber peer)
    {
        if (peer == callingPhoneNumber)
        {
            logger.logInfo("Call from ", peer, " was cancelled/dropped by the peer before accepting.");
            context.timer.stopTimer();
            context.user.showConnected();
            context.setState<ConnectedState>();
        }
        else
        {
            logger.logError("Received CallEnd from unexpected peer ", peer, " while waiting for call from ", callingPhoneNumber);
        }
    }

    void IncomingCallState::handleCallReject(common::PhoneNumber peer)
    {
        if (peer == callingPhoneNumber)
        {
            logger.logInfo("Caller cancelled the call before it was answered: ", peer);
            context.timer.stopTimer();
            context.user.showAlert("Call Cancelled", "Caller hung up before you answered.");
            context.setState<ConnectedState>();
        }
        else
        {
            logger.logError("Received CallReject from unexpected peer ", peer, " while expecting call from ", callingPhoneNumber);
        }
    }

    void IncomingCallState::handleSmsReceived(common::PhoneNumber from, std::string text)
    {
        logger.logInfo("SMS received during incoming call from: ", from);
        context.smsDb.addReceivedSms(from, text);
        context.user.showNewSms();
    }

    void IncomingCallState::handleCallRequest(common::PhoneNumber from)
    {
        if (from != callingPhoneNumber)
        {
            logger.logInfo("Received another CallRequest from ", from);
            context.bts.sendCallDropped(from);
        }
        else
        {
            logger.logDebug("Duplicate CallRequest from the same peer: ", from);
        }
    }

}