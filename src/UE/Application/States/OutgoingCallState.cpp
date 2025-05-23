#include "OutgoingCallState.hpp"
#include "IncomingCallState.hpp"
#include "ConnectedState.hpp"
#include "NotConnectedState.hpp"
#include <chrono>

namespace ue
{

    OutgoingCallState::OutgoingCallState(Context& context, common::PhoneNumber to)
        : BaseState(context, "OutgoingCallState"), callee(to)
    {
        logger.logInfo("Dialing: ", callee);
        context.user.showTalkingMobileScreen(to);
        context.timer.startTimer(std::chrono::minutes(2));
    }

    void OutgoingCallState::handleUiBack()
    {
        logger.logInfo("Call ended by user");
        context.timer.stopTimer();
        context.bts.sendCallDropped(callee);
        context.user.showAlert("Call ended", "Call ended by user");
        awaitingAfterEnd = true;
    }

    void OutgoingCallState::handleCallDropped(common::PhoneNumber to)
    {
        if (callee == to)
        {
            logger.logInfo("Call dropped by: ", to);
            context.timer.stopTimer();
            context.user.showAlert("Call ended", "Call ended by remote party");
            awaitingAfterEnd = true;
        }
    }

    void OutgoingCallState::handleDisconnected()
    {
        logger.logInfo("Lost connection during outgoing call");
        context.timer.stopTimer();
        context.user.showAlert("Disconnected", "Connection lost during outgoing call.");
        context.setState<NotConnectedState>();
    }

    void OutgoingCallState::handleUiAction(std::optional<std::size_t> ind)
    {
        if(awaitingAfterEnd)
        {
            logger.logInfo("Call ended, returning to menu");
            context.setState<ConnectedState>();
            awaitingAfterEnd = false;
            return;
        }

        std::string msg = context.user.getTextFromCall();
        if (msg.empty())
        {
            logger.logInfo("No message to send");
            context.user.showAlert("Error", "No message to send");
            return;
        }

        context.bts.sendTalkCall(callee, msg);
        context.user.deleteOutgoingText();
        context.timer.startTimer(std::chrono::minutes(2));
    }

    void OutgoingCallState::handleTalkCall(common::PhoneNumber from, const std::string &message)
    {
        if (from != callee)
        {
            logger.logInfo("Received message from unknown number: ", from);
            return;
        }
        context.user.addIncomingText(message);
        context.timer.startTimer(std::chrono::minutes(2));
    }

    void OutgoingCallState::handleTimeout()
    {
        logger.logInfo("Outgoing call timed out");
        context.timer.stopTimer();
        context.bts.sendCallDropped(callee);
        context.user.showAlert("Call ended", "Call ended due to timeout");
        context.setState<ConnectedState>();
        awaitingAfterEnd = true;
    }

    void OutgoingCallState::handleCallRequest(common::PhoneNumber from)
    {
        logger.logInfo("Received call request while in outgoing call");

        // Option: Drop current call and accept new one
        context.bts.sendCallDropped(callee);
        context.setState<IncomingCallState>(from);
    }

    void OutgoingCallState::handleNumberUnknown(common::PhoneNumber to)
    {
        if (callee == to)
        {
            logger.logInfo("Number unknown: ", to);
            context.timer.stopTimer();
            context.user.showAlert("Error", "Number unknown");
            context.setState<ConnectedState>();
        }
        else
        {
            logger.logInfo("Unknown number: ", to);
        }
    }

    void OutgoingCallState::handleSmsReceive(common::PhoneNumber from, std::string messageText)
    {
        logger.logInfo("Received SMS from: ", from);
        context.smsDatabase.addIncomingSms(from, messageText);
        context.user.showSms();
    }

}