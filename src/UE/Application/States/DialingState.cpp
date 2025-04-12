#include "DialingState.hpp"
#include "ConnectedState.hpp"
#include <chrono>

namespace ue
{

DialingState::DialingState(Context& context)
  : BaseState(context, "DialingState")
{
    context.user.showDialing();
    context.timer.startTimer(std::chrono::seconds(60));
}

void DialingState::handleUserAction(const std::string& id)
{
    logger.logDebug("DialingState handleUserAction: ", id);
    if(id == "ACCEPT") {
        // Pobieramy wprowadzony numer z GUI
        common::PhoneNumber dialed;
        bool success = context.user.getDialedNumber(dialed);
        if(success) {
            context.bts.sendCallRequest(dialed);
            logger.logInfo("Call request sent to ", dialed);
        } else {
            logger.logError("Failed to retrieve dialed phone number");
        }
    }
    else if(id == "REJECT") {
        logger.logInfo("User cancelled dialing. Transitioning back to ConnectedState.");
        context.setState<ConnectedState>();
    }
    else {
        BaseState::handleUserAction(id);
    }
}

void DialingState::handleTimeout() {
    logger.logInfo("DialingState timeout occurred. No response to call request.");
    context.setState<ConnectedState>();
}

void DialingState::handleCallAccepted(common::PhoneNumber from) {
    context.timer.stopTimer();
    logger.logInfo("Received CallAccepted. Transitioning to ConnectedState.");
    context.setState<ConnectedState>();
}

void DialingState::handleCallDropped(common::PhoneNumber from) {
    context.timer.stopTimer();
    logger.logInfo("Received CallDropped in DialingState. Transitioning to ConnectedState.");
    context.setState<ConnectedState>();
}

} // namespace ue
