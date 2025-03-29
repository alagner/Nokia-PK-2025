#include "ViewingSingleSmsState.hpp"
#include "ViewingSmsListState.hpp" // To go back
#include "NotConnectedState.hpp"
#include <stdexcept> // Include for exceptions

namespace ue
{

    ViewingSingleSmsState::ViewingSingleSmsState(Context &context, std::size_t smsIndex)
        : BaseState(context, "ViewingSingleSmsState"),
          viewingSmsIndex(smsIndex)
    {
        const auto &allSms = context.smsDb.getAllSms();
        if (viewingSmsIndex >= allSms.size())
        {
            logger.logError("Invalid SMS index provided: ", viewingSmsIndex);
            // Cannot recover easily here, maybe throw or go back to list?
            // Going back to list state for safety.
            context.setState<ViewingSmsListState>();
            // Throwing an exception might be better if this shouldn't happen
            // throw std::out_of_range("Invalid SMS index in ViewingSingleSmsState");
            return; // Return necessary if not throwing
        }

        const SmsMessage &message = allSms[viewingSmsIndex];
        logger.logInfo("Viewing SMS at index: ", viewingSmsIndex, ", From: ", message.from);

        // Mark as read *before* showing
        if (!message.isRead)
        {
            logger.logDebug("Marking SMS as read: index ", viewingSmsIndex);
            context.smsDb.markAsRead(viewingSmsIndex); // Mark as read
        }

        // Show the message content via UserPort
        context.user.showSmsView(message);
    }

    void ViewingSingleSmsState::handleUiBack()
    {
        logger.logInfo("Back action from single SMS view.");
        // Go back to the SMS list state
        context.setState<ViewingSmsListState>();
    }

    void ViewingSingleSmsState::handleDisconnected()
    {
        logger.logInfo("Connection lost while viewing single SMS.");
        context.setState<NotConnectedState>();
    }

    // Handle incoming SMS while viewing one
    void ViewingSingleSmsState::handleSmsReceived(common::PhoneNumber from, std::string text)
    {
        logger.logInfo("SMS received while viewing another SMS (from: ", from, ")");
        std::size_t smsIndex = context.smsDb.addSms(from, text);
        logger.logDebug("SMS stored at index: ", smsIndex);
        context.user.showNewSms();
        // No need to refresh the current view, user will see it when they go back to list
    }

} // namespace ue