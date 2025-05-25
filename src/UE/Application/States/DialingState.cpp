#include "DialingState.hpp"
#include "ConnectedState.hpp"
#include "NotConnectedState.hpp"
#include "OutgoingCallState.hpp"
#include "IncomingCallState.hpp"
#include <chrono>

using namespace std::chrono;

namespace ue
{

    DialingState::DialingState(Context &context)
        : BaseState(context, "DialingState"), awaitingAfterFail(false)
    {
        logger.logInfo("CallingState...");
        context.timer.startTimer(5s); // Start timer for 5 seconds
    }

    DialingState::~DialingState()
    {
        context.timer.stopTimer();
        diallingNumber = common::PhoneNumber{};  // Reset phone number
    }

    
    void DialingState::handleUiAction(std::optional<std::size_t>)
    {
        if (awaitingAfterFail)
        {
            logger.logInfo("Call failed, retrying...");
            context.user.showConnected();
            context.user.deleteOutgoingText();
            context.setState<ConnectedState>();

            return;
        }
        common::PhoneNumber recipient = context.user.getDialedPhoneNumber();
        printf("Dialing number: %s\n", common::to_string(recipient).c_str());

        logger.logInfo("Dialing started, calling number ", recipient);

        if (!recipient.isValid() || recipient == context.thisPhoneNumber)
        {
            logger.logError("Wrong phone number.");
            context.user.showAlert("Invalid number", "Please enter a valid number.");
            awaitingAfterFail = true;
            return;
        }

        diallingNumber = recipient;
        logger.logInfo("Dialing: ", diallingNumber);
        context.user.showAlert("Calling", "Dialing number:\n" + common::to_string(diallingNumber));
        context.bts.sendCallRequest(diallingNumber);
    }

    void DialingState::handleUiBack()
    {
        logger.logInfo("The call was cancelled.");
        context.bts.sendCallDropped(diallingNumber);
        context.user.showAlert("Call Cancelled", "The call was cancelled.");
        context.timer.stopTimer();
        awaitingAfterFail = true;
    }

    void DialingState::handleAcceptCall(common::PhoneNumber to)
    {
        logger.logInfo("Call accepted from: ", to);
        context.setState<OutgoingCallState>(to);
    }

    void DialingState::handleTimeout()
    {
        logger.logInfo("Call timed out");
        context.user.showAlert("Call Timeout", "Recipient did not answer.");
        awaitingAfterFail = true;
    }

    void DialingState::handleNumberUnknown(common::PhoneNumber to)
    {
        logger.logInfo("Unknown recipient: ", to);
        context.user.showAlert("Call Failed", "Number does not exist.");
        awaitingAfterFail = true;
    }

    void DialingState::handleCallReject(common::PhoneNumber to)
    {
        logger.logInfo("Call rejected by: ", to);
        context.setState<ConnectedState>();
    }

    void DialingState::handleDisconnected()
    {
        logger.logError("Disconnected while calling");
        context.setState<NotConnectedState>();
    }

    void DialingState::handleSmsReceive(common::PhoneNumber from, std::string messageText)
    {
        logger.logInfo("SMS received from number: ", from);
        std::size_t index = context.smsDatabase.addIncomingSms(from, messageText);
        logger.logDebug("SMS added to database with index: ", index);
        context.user.showSms();
    }

    void DialingState::handleCallRequest(common::PhoneNumber from)
    {
        logger.logInfo("Received call request while dialing");
        if (diallingNumber.isValid())
        {
            context.bts.sendCallDropped(diallingNumber);
        }
        context.setState<IncomingCallState>(from);
    }

    void DialingState::handleCallDropped(common::PhoneNumber from)
    {
         if (diallingNumber == from)
        {
            logger.logInfo("Call dropped by: ", from);
            context.timer.stopTimer();
            context.user.showAlert("Call Rejected", "Call was rejected by recipient.");
            context.user.showConnected();
            context.setState<ConnectedState>();
            awaitingAfterFail = true;
        }
    }

} // namespace ue