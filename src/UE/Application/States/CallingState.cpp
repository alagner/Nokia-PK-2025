#include "CallingState.hpp"
#include "TalkingState.hpp"
#include "ConnectedState.hpp"
#include "NotConnectedState.hpp"
#include "IncomingCallState.hpp"
#include <chrono>

using namespace std::chrono;

namespace ue
{

    CallingState::CallingState(Context &context)
        : BaseState(context, "CallingState"), awaitingUserAfterFailure(false)
    {
        logger.logInfo("CallingState...");
        context.timer.startTimer(60s);
    }

    void CallingState::handleUiAction(std::optional<std::size_t>)
    {
        if (awaitingUserAfterFailure)
        {
            logger.logInfo("User acknowledged call failure alert");
            context.setState<ConnectedState>();
            return;
        }

        common::PhoneNumber recipient = context.user.getDialedPhoneNumber();
        logger.logInfo("Dialed raw number: ", common::to_string(recipient));

        if (!recipient.isValid())
        {
            logger.logError("Wrong phone number.");
            context.user.showAlert("Invalid number", "Please enter a valid number.");
            awaitingUserAfterFailure = true;
            return;
        }

        dialedNumber = recipient;

        logger.logInfo("Dialing started, calling number ", dialedNumber);
        context.user.showAlert("Calling...", "Calling number: " + common::to_string(dialedNumber));
        context.bts.sendCallRequest(dialedNumber);
    }

    void CallingState::handleUiBack()
    {
        logger.logInfo("The call was cancelled.");
        context.bts.sendCallReject(dialedNumber);
        context.setState<ConnectedState>();
    }

    void CallingState::handleCallAccept(common::PhoneNumber peer)
    {
        logger.logInfo("Call picked up by: ", peer);
        context.setState<TalkingState>(peer);
    }

    void CallingState::handleTimeout()
    {
        logger.logInfo("Call timed out");
        context.user.showAlert("Call Timeout", "Recipient did not answer.");
        awaitingUserAfterFailure = true;
    }

    void CallingState::handleUnknownRecipient(common::PhoneNumber peer)
    {
        logger.logInfo("Call failed - Unknown recipient: ", peer);
        context.user.showAlert("Call Failed", "Number does not exist.");
        awaitingUserAfterFailure = true;
    }

    void CallingState::handleCallReject(common::PhoneNumber peer)
    {
        logger.logInfo("Call rejected by: ", peer);
        context.setState<ConnectedState>();
    }

    void CallingState::handleDisconnected()
    {
        logger.logError("Disconnected while calling");
        context.setState<NotConnectedState>();
    }

    void CallingState::handleSmsReceived(common::PhoneNumber from, std::string text)
    {
        logger.logInfo("SMS received from: ", from, " while calling");
        std::size_t smsIndex = context.smsDb.addReceivedSms(from, text);
        logger.logDebug("SMS stored at index: ", smsIndex);
        context.user.showNewSms();
    }

    void CallingState::handleCallRequest(common::PhoneNumber from)
    {
        logger.logInfo("Incoming call received from: ", from, " – cancelling outgoing call");

        if (dialedNumber.isValid())
        {
            context.bts.sendCallDropped(dialedNumber);
        }

        context.setState<IncomingCallState>(from);
    }

} // namespace ue
