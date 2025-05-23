#include "ViewingSmsListState.hpp"
#include "ViewingSmsState.hpp"
#include "ConnectedState.hpp"
#include "NotConnectedState.hpp"
#include "IncomingCallState.hpp"

namespace ue
{

ViewingSmsListState::ViewingSmsListState(Context& context)
    : BaseState(context, "ViewingSmsListState")
{
    logger.logDebug("Entering SMS list view");
    showList();
}

void ViewingSmsListState::showList()
{
    currentSmsList = context.smsDatabase.fetchAllMessages();

    if (currentSmsList.empty())
    {
        logger.logInfo("No SMS messages, returning to main menu");
        context.user.showAlert("Inbox Empty", "There are no SMS messages to display.");
        context.setState<ConnectedState>();
        return;
    }

    logger.logDebug("Preparing SMS list, total messages: ", currentSmsList.size());
    for (std::size_t i = 0; i < currentSmsList.size(); ++i)
    {
        const auto& sms = currentSmsList[i];
        bool isRead = (sms.direction == SmsMessage::Direction::Incoming && sms.status == SmsMessage::Status::ReceivedRead);
        logger.logDebug("Message ", i, ": ", common::to_string(sms.contact), ", Read: ", (isRead ? "Yes" : "No"));
    }

    context.user.showSmsList(currentSmsList);
}

void ViewingSmsListState::handleUiAction(std::optional<std::size_t> selectedIndex)
{
    if (!selectedIndex.has_value())
    {
        logger.logInfo("UI Action with no selection");
        return;
    }

    const std::size_t index = selectedIndex.value();

    if (currentSmsList.empty())
    {
        logger.logError("Unexpected empty SMS list when user selected index ", index);
        context.user.showAlert("Error", "No messages available.");
        return;
    }

    if (index >= currentSmsList.size())
    {
        logger.logError("Selected index ", index, " is out of range (list size ", currentSmsList.size(), ")");
        return;
    }

    logger.logInfo("Opening SMS at index: ", index);
    context.setState<ViewingSmsState>(index);
}

void ViewingSmsListState::handleUiBack()
{
    logger.logInfo("Back pressed - returning to main menu");
    context.setState<ConnectedState>();
}

void ViewingSmsListState::handleDisconnected()
{
    logger.logInfo("Disconnected while viewing SMS list");
    context.setState<NotConnectedState>();
}

void ViewingSmsListState::handleSib(common::BtsId btsId)
{
    logger.logInfo("Ignoring unexpected SIB in SMS list view, BTS ID: ", btsId);
}

void ViewingSmsListState::handleSmsReceive(common::PhoneNumber from, std::string messageText)
{
    logger.logInfo("New SMS received while browsing list, sender: ", from);
    std::size_t newIndex = context.smsDatabase.addIncomingSms(from, messageText);
    logger.logDebug("Stored new SMS at index ", newIndex);
    context.user.showSms();

    // Refresh the SMS list
    showList();
}

void ViewingSmsListState::handleCallRequest(common::PhoneNumber from)
{
    logger.logInfo("Incoming call request while viewing SMS list, caller: ", from);
    context.setState<IncomingCallState>(from);

}
} // namespace ue
