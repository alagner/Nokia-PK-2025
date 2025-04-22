#include "CallingState.hpp"
#include "TalkingState.hpp"
#include "ConnectedState.hpp"
#include "NotConnectedState.hpp"
#include <chrono>

using namespace std::chrono;

namespace ue
{

    CallingState::CallingState(Context &context)
        : BaseState(context, "CallingState")
    {
        logger.logInfo("CallingState...");
        context.timer.startTimer(60s);
    }

    void CallingState::handleUiAction(std::optional<std::size_t>)
    {
        common::PhoneNumber recipent = context.user.getSmsRecipient();

        logger.logInfo("Dialing started, calling number ", recipent);

        if (!recipent.isValid())
        {
            logger.logError("Wrong phone number.");
            context.user.showAlert("Invalid number", "Please enter a valid number.");
            return;
        }

        context.bts.sendCallRequest(recipent);
    }

    void CallingState::handleUiBack()
    {
        logger.logInfo("The call was cancelled.");
        context.bts.sendCallEnd(dialedNumber);
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
        context.setState<ConnectedState>();
    }

    void CallingState::handleCallDropped()
    {
        logger.logInfo("Call dropped by BTS");
        context.user.showAlert("Call Failed", "Recipient dropped the call.");
        context.setState<ConnectedState>();
    }

    void CallingState::handleUnknownRecipient(common::PhoneNumber peer)
    {
        logger.logInfo("Call failed - Unknown recipient: ", peer);
        context.user.showAlert("Call Failed", "Number does not exist.");
        context.setState<ConnectedState>();
    }

    void CallingState::handleCallReject(common::PhoneNumber peer)
    {
        logger.logInfo("Call rejected by: ", peer);
        context.user.showAlert("Call Rejected", "The recipient rejected your call.");
        context.setState<ConnectedState>();
    }

    void CallingState::handleDisconnected()
    {
        logger.logError("Disconnected while calling");
        context.setState<NotConnectedState>();
    }

} // namespace ue
