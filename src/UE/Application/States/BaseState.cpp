#include "BaseState.hpp"

namespace ue{

BaseState::BaseState(Context &context, const std::string &name)
    : context(context),
      logger(context.logger, "[" + name + "]"){
    logger.logDebug("entry");
}

BaseState::~BaseState(){
    logger.logDebug("exit");
}

void BaseState::handleTimeout(){
    logger.logError("Unexpected: handleTimeout");
}

void BaseState::handleSib(common::BtsId btsId){
    logger.logError("Unexpected: handleSib: ", btsId);
}

void BaseState::handleAttachAccept(){
    logger.logError("Unexpected: handleAttachAccept");
}

void BaseState::handleAttachReject(){
    logger.logError("Unexpected: handleAttachReject");
}
void BaseState::handleDisconnected(){
    logger.logError("Unexpected: handleDisconnected");
}

void BaseState::handleMessageReceive(common::PhoneNumber sender, std::string text){
    logger.logError("Unexpected: handleMessageReceive() from: ", sender, " content: ", text);
}

void BaseState::handleMessageSentResult(common::PhoneNumber to, bool success){
    logger.logError("Unexpected: handleMessageSentResult for: ", to, ", Success: ", success);
}

void BaseState::handleMessageComposeResult(common::PhoneNumber reciver, const std::string &text){
    logger.logError("Unexpected: handleMessageComposeResult for: ", reciver, ", text: ", text);
}

void BaseState::handleUiAction(std::optional<std::size_t> selectedIndex){
    std::string indexStr = selectedIndex.has_value() ? std::to_string(selectedIndex.value()) : "none";
    logger.logError("Unexpected: handleUiAction, index: ", indexStr);
}

void BaseState::handleUiBack(){
    logger.logError("Unexpected: handleUiBack");
}

// void BaseState::handleCallRequest(common::PhoneNumber from) {
//     logger.logError("Unexpected: handleCallRequest");
// }

}
