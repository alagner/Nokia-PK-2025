#include "ConnectedState.hpp"
#include "NotConnectedState.hpp"
#include "ComposeSmsState.hpp"
#include "ViewListSmsState.hpp"

namespace ue{

ConnectedState::ConnectedState(Context &context)
    : BaseState(context, "ConnectedState"){
    context.user.showConnected();
}

void ConnectedState::handleDisconnected(){
    context.setState<NotConnectedState>();
}

void ConnectedState::handleMessageReceive(common::PhoneNumber sender, std::string text){
    logger.logInfo("SMS received from: ", sender, ", content : ", text);

    std::size_t smsIndex = context.smsStorage.addMessage(sender, text);
    logger.logDebug("SMS stored at index: ", smsIndex);

    context.user.showNewMessage();
}

void ConnectedState::handleUiAction(std::optional<std::size_t> selectedIndex){
    if (!selectedIndex.has_value()) {
        logger.logInfo("UI Action received with no index in Main Menu");
        return;
    }

    logger.logInfo("Main menu selection: index=", selectedIndex.value());

    switch (selectedIndex.value()){
        case 0:
            logger.logInfo("Compose SMS selected");
            context.setState<ComposeSmsState>();
            break;

        case 1:
            logger.logInfo("View SMS selected");
            context.setState<ViewListSmsState>();
            break;

        case 2:
            logger.logInfo("Call selected");
            // TODO: Set state to CallState or similar to handle call menu
            // context.setState<CallState>();
            break;

        default:
            logger.logError("Invalid menu option selected: ", selectedIndex.value());
            break;
    }
}

void ConnectedState::handleUiBack(){
    logger.logInfo("Back action in main menu - ignored");
}

    void ConnectedState::handleMessageSentResult(common::PhoneNumber to, bool success){
    logger.logInfo("Received SMS send result for ", to, " while in main menu. Success: ", success);
    if (!success) {
        if (!context.smsStorage.markSmsOutFailed()) {
            logger.logInfo("Could not mark last outgoing SMS as failed.");
        }
        context.user.showNotify("SMS Failed", "Could not send SMS to " + common::to_string(to));
    }
}

}
