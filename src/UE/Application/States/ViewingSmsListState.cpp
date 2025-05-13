#include "ViewingSmsListState.hpp"
#include "ViewingSingleSmsState.hpp"
#include "ConnectedState.hpp"
#include "NotConnectedState.hpp"
#include "IncomingCallState.hpp"

namespace ue
{
    ViewingSmsListState::ViewingSmsListState(Context &context)
        : BaseState(context, "ViewingSmsListState")
    {
        logger.logDebug("Entering SMS List view");
        // Display the list of SMS messages
        showList();
    }

    void ViewingSmsListState::showList()
    {
        currentSmsList = context.smsDb.getAllSms(); // Fetch all SMS
        if (currentSmsList.empty())
        {
            logger.logInfo("SMS list is empty.");
            // Option 1: Show an alert (if implemented)
            context.user.showAlert("SMS Inbox", "No messages.");
            // Option 2: Go back immediately
            context.setState<ConnectedState>();
        }
        else
        {
            logger.logDebug("Displaying SMS list with updated read statuses:");
            for (size_t i = 0; i < currentSmsList.size(); ++i)
            {
                const auto &msg = currentSmsList[i];
                bool isRead = (msg.direction == SmsMessage::Direction::INCOMING) ? (msg.status == SmsMessage::Status::RECEIVED_READ) : true;
                logger.logDebug("SMS #", i, ": ", common::to_string(msg.peer),
                                " - Read: ", isRead ? "Yes" : "No");
            }

            context.user.showSmsList(currentSmsList);
        }
    }

    void ViewingSmsListState::handleUiAction(std::optional<std::size_t> selectedIndex)
    {
        if (!selectedIndex.has_value())
        {
            logger.logInfo("UI Action received with no index in SMS List");
            return;
        }

        std::size_t index = selectedIndex.value();
        if (currentSmsList.empty())
        {
            logger.logError("Cannot select SMS from empty list");
            context.user.showAlert("Error", "No messages available");
            return;
        }
        if (index < currentSmsList.size())
        {
            logger.logInfo("SMS selected at index: ", index);
            // Transition to view the selected SMS, passing the index
            context.setState<ViewingSingleSmsState>(index);
        }
        else
        {
            logger.logError("Selected index out of bounds: ", index, " (List size: ", currentSmsList.size(), ")");
            // Maybe show the list again or go back? For now, just log error.
        }
    }

    void ViewingSmsListState::handleUiBack()
    {
        logger.logInfo("Back action from SMS List view.");

        logger.logDebug("Explicitly refreshing SMS list before exiting");

        context.setState<ConnectedState>(); // Go back to main menu
    }

    void ViewingSmsListState::handleDisconnected()
    {
        logger.logInfo("Connection lost while viewing SMS list.");
        context.setState<NotConnectedState>();
    }

    // Handle incoming SMS while viewing the list
    void ViewingSmsListState::handleSmsReceived(common::PhoneNumber from, std::string text)
    {
        logger.logInfo("SMS received while viewing list (from: ", from, ")");
        std::size_t smsIndex = context.smsDb.addReceivedSms(from, text);
        logger.logDebug("SMS stored at index: ", smsIndex);
        context.user.showNewSms();
        // Refresh the list view
        showList();
    }

    void ViewingSmsListState::handleCallRequest(common::PhoneNumber from)
    {
        logger.logInfo("Incoming call while viewing SMS list from: ", from);
        context.setState<IncomingCallState>(from);
    }

}