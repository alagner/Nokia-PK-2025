#include "ViewSmsState.hpp"
#include "ViewListSmsState.hpp"
#include "NotConnectedState.hpp"

namespace ue {

ViewSmsState::ViewSmsState(Context& ctx, std::size_t smsIndex)
    : BaseState(ctx, "ViewSmsState")
    , smsIndex_(smsIndex)
{
    const auto& inbox = context.smsStorage.getAllMessages();
    if (smsIndex_ >= inbox.size()) {
        logger.logError("Invalid SMS index: ", smsIndex_);
        returnToSmsList();
        return;
    }

    const auto& msg = inbox[smsIndex_];
    logger.logInfo("Opening SMS #", smsIndex_, " from ", msg.sender);
    if (msg.direction == SmsMessage::Dir::in
        && msg.status == SmsMessage::Status::receiveUR)
    {
        logger.logDebug("Marking as read SMS #", smsIndex_);
        context.smsStorage.markAsRead(smsIndex_);
    }

    displayCurrentSms();
}

void ViewSmsState::displayCurrentSms()
{
    const auto& inbox = context.smsStorage.getAllMessages();
    context.user.showMessageView(inbox[smsIndex_]);
}

void ViewSmsState::handleUiBack()
{
    logger.logInfo("Back from single SMS view");
    returnToSmsList();
}

void ViewSmsState::handleUiAction(std::optional<std::size_t> selectedIndex)
{
    logger.logInfo("UI action in SMS view, returning to list");
    returnToSmsList();
}

void ViewSmsState::returnToSmsList()
{
    const auto& inbox = context.smsStorage.getAllMessages();
    context.user.showListMessage(inbox);
    context.setState<ViewListSmsState>();
}

void ViewSmsState::handleDisconnected()
{
    logger.logInfo("Lost connection while viewing SMS");
    context.setState<NotConnectedState>();
}

void ViewSmsState::handleMessageReceive(common::PhoneNumber from, std::string text)
{
    logger.logInfo("New SMS arrived (from=", from, ") while viewing another");
    auto newIdx = context.smsStorage.addMessage(from, text);
    logger.logDebug("Stored new SMS at index=", newIdx);
    context.user.showNewMessage();
}

}
