#include "ViewListSmsState.hpp"
#include "ViewSmsState.hpp"
#include "ConnectedState.hpp"
#include "NotConnectedState.hpp"

namespace ue{
    ViewListSmsState::ViewListSmsState(Context& context)
        : BaseState(context, "ViewListSmsState"){
        logger.logDebug("Entering SMS List view");
        showList();
    }

    void ViewListSmsState::showList(){
        currentSmsList = context.smsStorage.getAllMessages();
        if (currentSmsList.empty()){
            logger.logInfo("SMS list is empty.");
            context.user.showNotify("SMS Inbox", "No messages.");
            context.setState<ConnectedState>();
        }
        else{
            logger.logDebug("Displaying SMS list with updated read statuses:");
            for (size_t i = 0; i < currentSmsList.size(); ++i){
                const auto &msg = currentSmsList[i];
                bool isRead = (msg.direction == SmsMessage::Dir::in) ? (msg.status == SmsMessage::Status::receiveUR) : true;
                logger.logDebug("SMS #", i, ": ", common::to_string(msg.sender),
                                " - Read: ", isRead ? "Yes" : "No");
            }

            context.user.showListMessage(currentSmsList);
        }
    }

    void ViewListSmsState::handleUiAction(std::optional<std::size_t> selectedIndex){
        if (!selectedIndex.has_value()){
            logger.logInfo("UI Action received with no index in SMS List");
            return;
        }

        std::size_t index = selectedIndex.value();
        if (currentSmsList.empty()){
            logger.logError("Cannot select SMS from empty list");
            context.user.showNotify("Error", "No messages available");
            return;
        }
        if (index < currentSmsList.size()){
            logger.logInfo("SMS selected at index: ", index);
            context.setState<ViewSmsState>(index);
        }
        else{
            logger.logError("Selected index out of bounds: ", index, " (List size: ", currentSmsList.size(), ")");
        }
    }

    void ViewListSmsState::handleUiBack(){
        logger.logInfo("Back action from SMS List view.");

        logger.logDebug("Explicitly refreshing SMS list before exiting");

        context.setState<ConnectedState>();
    }

    void ViewListSmsState::handleDisconnected(){
        logger.logInfo("Connection lost while viewing SMS list.");
        context.setState<NotConnectedState>();
    }

    void ViewListSmsState::handleMessageReceive(common::PhoneNumber from, std::string text){
        logger.logInfo("SMS received while viewing list (from: ", from, ")");
        std::size_t smsIndex = context.smsStorage.addMessage(from, text);
        logger.logDebug("SMS stored at index: ", smsIndex);
        context.user.showNewMessage();
        showList();
    }
}