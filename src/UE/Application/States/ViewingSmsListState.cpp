#include "ViewingSmsListState.hpp"
#include "ViewingSingleSmsState.hpp"
#include "ConnectedState.hpp"
#include "NotConnectedState.hpp"

namespace ue
{
    ViewingSmsListState::ViewingSmsListState(Context &context)
        : BaseState(context, "ViewingSmsListState")
    {
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
            context.user.showSmsList(currentSmsList); // Display via UserPort
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
        std::size_t smsIndex = context.smsDb.addSms(from, text);
        logger.logDebug("SMS stored at index: ", smsIndex);
        context.user.showNewSms();
        // Refresh the list view
        showList();
    }
}