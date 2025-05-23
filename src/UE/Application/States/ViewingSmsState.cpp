#include "ViewingSmsState.hpp"
#include "ViewingSmsListState.hpp"
#include "NotConnectedState.hpp"
#include "IncomingCallState.hpp"
#include <stdexcept>

namespace ue
{

    ViewingSmsState::ViewingSmsState(Context& context, std::size_t smsIndex)
        : BaseState(context, "ViewingSmsState"),
          viewingSmsIndex(smsIndex)
    {
        const auto& allMessages = context.smsDatabase.fetchAllMessages();

        if (viewingSmsIndex >= allMessages.size())
        {
            logger.logError("Invalid index: ", viewingSmsIndex, ", list size: ", allMessages.size());
            context.setState<ViewingSmsListState>();
            return;
        }

        const SmsMessage& selectedMessage = allMessages[viewingSmsIndex];

        logger.logInfo("Opening SMS from: ", selectedMessage.contact, ", index: ", viewingSmsIndex);

        if (selectedMessage.direction == SmsMessage::Direction::Incoming &&
            selectedMessage.status == SmsMessage::Status::ReceivedUnread)
        {
            logger.logDebug("Marking SMS as read at ind: ", viewingSmsIndex);
            context.smsDatabase.markAsRead(viewingSmsIndex);
        }

        context.user.showSmsView(selectedMessage);
    }

    void ViewingSmsState::handleSib(common::BtsId btsId)
    {
        logger.logInfo("Ignoring SIB in ViewingSmsState from BTS: ", btsId);
    }

    void ViewingSmsState::handleUiBack()
    {
        const auto &smsStorage = context.smsDatabase.fetchAllMessages();
        context.user.showSmsList(smsStorage);

        logger.logInfo("Returning to SMS list from message view");
        context.setState<ViewingSmsListState>();
    }

    void ViewingSmsState::handleDisconnected()
    {
        logger.logInfo("Disconnected from BTS while viewing a message");
        context.setState<NotConnectedState>();
    }

    void ViewingSmsState::handleSmsReceive(common::PhoneNumber from, std::string messageText)
    {
        logger.logInfo("New SMS received while viewing another message. Sender: ", from);
        std::size_t index = context.smsDatabase.addIncomingSms(from, messageText);

        logger.logDebug("Saved incoming SMS at index: ", index);
        context.user.showSms();
    }

    void ViewingSmsState::handleUiAction(std::optional<std::size_t> selectedIndex)
    {
        if (!selectedIndex.has_value())
        {
            logger.logInfo("No index - switching to compose");
            return;
        }

        logger.logInfo("Returning to list");
        const auto &allSms = context.smsDatabase.fetchAllMessages();

        context.user.showSmsList(allSms);
        context.setState<ViewingSmsListState>();
    }

    void ViewingSmsState::handleCallRequest(common::PhoneNumber from)
    {
        logger.logInfo("Incoming call request from: ", from);
        context.user.showIncomingCall(from);
        context.setState<IncomingCallState>(from);
    }

}
