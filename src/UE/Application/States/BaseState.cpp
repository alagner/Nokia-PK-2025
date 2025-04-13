#include "BaseState.hpp"
#include "NotConnectedState.hpp" // Include for default disconnect handling if needed

namespace ue
{

BaseState::BaseState(Context &context, const std::string &name)
    : context(context),
      logger(context.logger, "[" + name + "]")
{
    logger.logDebug("entry");
}

BaseState::~BaseState()
{
    logger.logDebug("exit");
}

void BaseState::handleTimeout()
{
    logger.logError("Uexpected: handleTimeout");
}

void BaseState::handleSib(common::BtsId btsId)
{
    logger.logError("Uexpected: handleSib: ", btsId);
}

void BaseState::handleAttachAccept()
{
    logger.logError("Uexpected: handleAttachAccept");
}

void BaseState::handleAttachReject()
{
    logger.logError("Uexpected: handleAttachReject");
}

void BaseState::handleDisconnect()
{
    // This is handled globally in Application::handleDisconnect now for consistency
    // Log message will automatically include the state name prefix
    logger.logInfo("Disconnect event received"); // CORRECTED: Removed redundant logger.getPrefix()
    // State-specific cleanup could happen here if needed, but transition is handled by Application
}

void BaseState::handleSms(const common::PhoneNumber& from, const std::string& text)
{
    logger.logError("Unexpected: handleSms from: ", from, " text: ", text);
}

void BaseState::handleUserAction(const std::string& id)
{
    logger.logError("Unexpected user action: ", id);
}

void BaseState::handleCallRequest(common::PhoneNumber from)
{
    logger.logError("Unexpected call request in this state");
}

void BaseState::handleCallAccepted(common::PhoneNumber from)
{
    logger.logError("Unexpected call accepted in this state");
}

void BaseState::handleCallDropped(common::PhoneNumber from)
{
    logger.logError("Unexpected call dropped in this state");
}

void BaseState::handleCallTalk(common::PhoneNumber from, const std::string& text)
{
    logger.logError("Unexpected call talk in this state");
}

void BaseState::handleUnknownRecipient(common::MessageId originalMessageId, common::PhoneNumber from)
{
    logger.logError("Unexpected unknown recipient in this state: ", to_string(originalMessageId));
}

}