#include "ViewSmsState.hpp"
#include "ViewListSmsState.hpp"
#include "NotConnectedState.hpp"
#include <stdexcept>

namespace ue{

    ViewSmsState::ViewSmsState(Context &context, std::size_t smsIndex)
        : BaseState(context, "ViewSmsState"),
          viewSmsIndex(smsIndex){
        const auto &allSms = context.smsStorage.getAllMessages();
        if (viewSmsIndex >= allSms.size()){
            logger.logError("Invalid SMS index provided: ", viewSmsIndex);
            context.setState<ViewListSmsState>();
            return;
        }

        const SmsMessage &message = allSms[viewSmsIndex];
        logger.logInfo("Viewing SMS at index: ", viewSmsIndex, ", From: ", message.sender);

        if (message.direction == SmsMessage::Dir::in &&
            message.status == SmsMessage::Status::receiveUR){
            logger.logDebug("Marking SMS as read: index ", viewSmsIndex);
            context.smsStorage.markAsRead(viewSmsIndex);
        }

        context.user.showMessageView(message);
    }

    void ViewSmsState::handleUiBack(){
        const auto &allSms = context.smsStorage.getAllMessages();
        context.user.showListMessage(allSms);

        context.setState<ViewListSmsState>();
    }

    void ViewSmsState::handleUiAction(std::optional<std::size_t> selectedIndex){
        if (!selectedIndex.has_value()){
            logger.logInfo("Action without index in single SMS view - switching to compose");
            return;
        }

        logger.logInfo("UI action in single SMS view - returning to list");
        const auto &allSms = context.smsStorage.getAllMessages();
        context.user.showListMessage(allSms);
        context.setState<ViewListSmsState>();
    }

    void ViewSmsState::handleDisconnected(){
        logger.logInfo("Connection lost while viewing single SMS.");
        context.setState<NotConnectedState>();
    }

    void ViewSmsState::handleMessageReceive(common::PhoneNumber from, std::string text){
        logger.logInfo("SMS received while viewing another SMS (from: ", from, ")");
        std::size_t smsIndex = context.smsStorage.addMessage(from, text);
        logger.logDebug("SMS stored at index: ", smsIndex);
        context.user.showNewMessage();
    }

}