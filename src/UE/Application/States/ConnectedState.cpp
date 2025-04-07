#include "ConnectedState.hpp"
#include "NotConnectedState.hpp"
#include "Application.hpp"
#include "ViewingSmsListState.hpp" // CORRECTED: Include header for transition

namespace ue
{

ConnectedState::ConnectedState(Context &context)
    : BaseState(context, "ConnectedState")
{
    context.user.showConnected();
    context.app.updateSmsIndicator();
}

void ConnectedState::handleSms(const common::PhoneNumber& from, const std::string& text)
{
    logger.logInfo("Received SMS from: ", from, " while in Connected state.");
    context.app.storeReceivedSms(from, text);
}

void ConnectedState::handleUserAction(const std::string& id)
{
    logger.logDebug("Handling user action: ", id);
    if (id == "sms.view")
    {
        logger.logInfo("User selected View SMS. Transitioning to ViewingSmsListState.");
        // CORRECTED: Add state transition
        context.setState<ViewingSmsListState>();
    }
    // Add handlers for "sms.compose", "call.dial", "ACCEPT", "REJECT" later
    else if (id == "ACCEPT" || id == "REJECT")
    {
        logger.logInfo("Ignoring Accept/Reject in main menu state.");
        // Stay in this state, maybe refresh view if needed
        context.user.showConnected();
    }
    else
    {
        BaseState::handleUserAction(id); // Default handling (log error)
    }
}

} // namespace ue