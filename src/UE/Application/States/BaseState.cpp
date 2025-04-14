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
    logger.logError("Uexpected: handleTimeout");
}

void BaseState::handleSib(common::BtsId btsId){
    logger.logError("Uexpected: handleSib: ", btsId);
}

void BaseState::handleAttachAccept(){
    logger.logError("Uexpected: handleAttachAccept");
}

void BaseState::handleAttachReject(){
    logger.logError("Uexpected: handleAttachReject");
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

}
