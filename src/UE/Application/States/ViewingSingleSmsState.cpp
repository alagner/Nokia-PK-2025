#include "ViewingSingleSmsState.hpp"
#include "ViewingSmsListState.hpp" // To go back
#include "NotConnectedState.hpp"
#include <stdexcept> // Include for exceptions
#include "IncomingCallState.hpp"

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
            context.setState<ViewingSmsListState>();
            return;
        }

        const SmsMessage &message = allSms[viewingSmsIndex];
        logger.logInfo("Viewing SMS at index: ", viewingSmsIndex, ", From: ", message.peer);

        if (message.direction == SmsMessage::Direction::INCOMING &&
            message.status == SmsMessage::Status::RECEIVED_UNREAD)
        {
            logger.logDebug("Marking SMS as read: index ", viewingSmsIndex);
            context.smsDb.markAsRead(viewingSmsIndex);
        }

        // Show the message content via UserPort
        context.user.showSmsView(message);
    }

    void ViewingSingleSmsState::handleUiBack()
    {
        // Pobierz aktualne wiadomości i pokaż listę ponownie przed zmianą stanu
        const auto &allSms = context.smsDb.getAllSms();
        context.user.showSmsList(allSms);

        context.setState<ViewingSmsListState>();
    }

    void ViewingSingleSmsState::handleUiAction(std::optional<std::size_t> selectedIndex)
    {
        if (!selectedIndex.has_value())
        {
            logger.logInfo("Action without index in single SMS view - switching to compose");
            return;
        }

        logger.logInfo("UI action in single SMS view - returning to list");
        const auto &allSms = context.smsDb.getAllSms();
        context.user.showSmsList(allSms);
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
        std::size_t smsIndex = context.smsDb.addReceivedSms(from, text);
        logger.logDebug("SMS stored at index: ", smsIndex);
        context.user.showNewSms();
        // No need to refresh the current view, user will see it when they go back to list
    }

    void ViewingSingleSmsState::handleCallRequest(common::PhoneNumber from)
    {
        logger.logInfo("Incoming call while reading SMS from: ", from);
        context.setState<IncomingCallState>(from);
    }

} // namespace ue