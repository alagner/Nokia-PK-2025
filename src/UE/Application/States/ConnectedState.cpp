#include "ConnectedState.hpp"
#include "NotConnectedState.hpp"
#include "Application.hpp"
#include "ViewingSmsListState.hpp"
#include "ComposingSmsState.hpp"
#include "DialingState.hpp"
#include "ReceivingCallState.hpp"

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
        context.setState<ViewingSmsListState>();
    }
    else if (id == "sms.compose")
    {
        logger.logInfo("User selected Compose SMS. Transitioning to ComposingSmsState.");
        context.setState<ComposingSmsState>();
    }
    // Add handlers for "call.dial" later
    else if (id == "ACCEPT" || id == "REJECT")
    {
        logger.logInfo("Ignoring Accept/Reject in main menu state.");
        context.user.showConnected(); // Refresh potentially?
    }
    else
    {
        BaseState::handleUserAction(id);
    }
}

void ConnectedState::handleCallRequest(common::PhoneNumber from)
{
    logger.logInfo("Received Call Request from: ", from, ". Transitioning to ReceivingCallState.");
    context.setState<ReceivingCallState>(from);
}

void ConnectedState::handleCallAccepted(common::PhoneNumber from)
{
    logger.logError("Unexpected CallAccepted in ConnectedState.");
}

void ConnectedState::handleCallDropped(common::PhoneNumber from)
{
    logger.logError("Unexpected CallDropped in ConnectedState.");
}

void ConnectedState::handleCallTalk(common::PhoneNumber from, const std::string& text)
{
    logger.logError("Unexpected CallTalk in ConnectedState.");
}

}