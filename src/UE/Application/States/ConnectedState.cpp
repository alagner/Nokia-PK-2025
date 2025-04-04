#include "ConnectedState.hpp"
#include "NotConnectedState.hpp"

namespace ue
{

ConnectedState::ConnectedState(Context &context)
    : BaseState(context, "ConnectedState")
{
    context.user.showConnected();
}

void ConnectedState::handleDisconnected(){
    context.setState<NotConnectedState>();
}

void ConnectedState::handleSmsReceive(common::PhoneNumber sender, std::string text){
    logger.logInfo("SMS received from: ", sender, ", content : ", text);

    std::size_t smsIndex = context.smsStorage.addMessage(sender, text);
    logger.logDebug("SMS stored at index: ", smsIndex);

    context.user.showNewMessage();
}

}
