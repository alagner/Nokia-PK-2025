#include "DialingState.hpp"
#include "ConnectedState.hpp"
#include "NotConnectedState.hpp"
#include <chrono>

using namespace std::chrono;

namespace ue
{

    DialingState::DialingState(Context &context)
        : BaseState(context, "CallingState")
    {
        logger.logInfo("CallingState...");
        context.timer.startTimer(60s);
    }

    void DialingState::handleUiAction(std::optional<std::size_t>)
    {
        // Change from getSmsRecipient() to getDialedPhoneNumber()
        common::PhoneNumber recipient = context.user.getDialedPhoneNumber();
        printf("Dialing number: %s\n", common::to_string(recipient).c_str());

        logger.logInfo("Dialing started, calling number ", recipient);

        if (!recipient.isValid())
        {
            logger.logError("Wrong phone number.");
            context.user.showAlert("Invalid number", "Please enter a valid number.");
            return;
        }

        context.bts.sendCallRequest(recipient);
    }

    void DialingState::handleUiBack()
    {
        logger.logInfo("The call was cancelled.");
        context.bts.sendCallDropped(diallingNumber);
        context.setState<ConnectedState>();
    }

//    void DialingState::handleCallAccept(common::PhoneNumber peer)
//    {
//        logger.logInfo("Call picked up by: ", peer);
//        context.setState<TalkingState>(peer);
//    }

    void DialingState::handleTimeout()
    {
        logger.logInfo("Call timed out");
        context.user.showAlert("Call Timeout", "Recipient did not answer.");
        context.setState<ConnectedState>();
    }

//    void DialingState::handleUnknownRecipient(common::PhoneNumber peer)
//    {
//        logger.logInfo("Call failed - Unknown recipient: ", peer);
//        context.user.showAlert("Call Failed", "Number does not exist.");
//        context.setState<ConnectedState>();
//    }
//
//    void DialingState::handleCallReject(common::PhoneNumber peer)
//    {
//        logger.logInfo("Call rejected by: ", peer);
//        context.setState<ConnectedState>();
//    }

    void DialingState::handleDisconnected()
    {
        logger.logError("Disconnected while calling");
        context.setState<NotConnectedState>();
    }

} // namespace ue