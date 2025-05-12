#include "ConnectedState.hpp"
#include "NotConnectedState.hpp"
#include "ViewingSmsListState.hpp"
#include "ComposingSmsState.hpp"

namespace ue
{

ConnectedState::ConnectedState(Context &context)
    : BaseState(context, "ConnectedState")
{
    logger.logInfo("Entered ConnectedState");
    context.user.showConnected();
}

void ConnectedState::handleDisconnected()
{
    logger.logInfo("Lost connection with BTS – switching to NotConnectedState");
    context.setState<NotConnectedState>();
}

void ConnectedState::handleSmsReceive(common::PhoneNumber from, std::string messageText)
{
    logger.logInfo("Incoming SMS from ", from);
    const std::size_t newIndex = context.smsDatabase.addIncomingSms(from, std::move(messageText));

    logger.logDebug("Saved new SMS at index ", newIndex);
    context.user.showSms(); // triggers notification in UI
}

void ConnectedState::showMenu()
{
    logger.logInfo("(Re-showing) Menu");
    context.user.showConnected();
}

void ConnectedState::handleUiAction(std::optional<std::size_t> selectedIndex)
{
    if (!selectedIndex)
    {
        logger.logInfo("No menu item selected - ignoring");
        return;
    }

    const std::size_t index = *selectedIndex;
    logger.logInfo("Main Menu selection index: ", index);

    switch (index)
    {
    case 0:
        logger.logInfo("User chose: Create New SMS (not implemented)");
        context.setState<ComposingSmsState>();
        break;
    case 1:
        logger.logInfo("User chose: Open SMS Inbox");
        context.setState<ViewingSmsListState>();
        break;
    default:
        logger.logInfo("Unexpected menu index: ", index);
        break;
    }
}

void ConnectedState::handleSmsSent(common::PhoneNumber to, bool succeed)
{
    logger.logInfo("Received sms sending for ", to, " while in main menu. Success: ", succeed);
    if (!succeed) {
        if (!context.smsDatabase.markLastOutgoingAsFailed()) {
            logger.logInfo("Cant mark last outgoing SMS  as failed.");
        }
        context.user.showAlert("SMS Failed", "Could not send SMS to " + common::to_string(to));
    }
}

void ConnectedState::handleUiBack()
{
    logger.logInfo("Back button pressed on main menu – no action defined");
}

void ConnectedState::handleSib(common::BtsId btsId)
{
    logger.logInfo("Ignoring SIB in ConnectedState from BTS: ", btsId);
}



}
