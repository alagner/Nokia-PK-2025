#include "BaseState.hpp"

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

void BaseState::viewSmsList()
{
    logger.logError("Unexpected: viewSmsList");
}

void BaseState::viewSms(unsigned int)
{
    logger.logError("Unexpected: viewSms");
}

void BaseState::sendSms(const SmsEntity& sms)
{
    logger.logError("Unexpected: sendSms");
}

void BaseState::composeSms()
{
    logger.logError("Unexpected: composeSms");
}

void BaseState::handleTimeout()
{
    logger.logError("Unexpected: handleTimeout");
}

void BaseState::handleSib(common::BtsId btsId)
{
    logger.logError("Unexpected: handleSib: ", btsId);
}

void BaseState::handleAttachAccept()
{
    logger.logError("Unexpected: handleAttachAccept");
}

void BaseState::handleAttachReject()
{
    logger.logError("Unexpected: handleAttachReject");
}

void BaseState::handleDisconnect()
{
    logger.logError("Unexpected: handleDisconnect");
}

void BaseState::handleSms(common::PhoneNumber from, std::string text)
{
    logger.logError("Uexpected: handleSms");
}

void BaseState::handleSmsDeliveryFailure(common::PhoneNumber)
{
    logger.logError("Unexpected: handleSmsDeliveryFailure");
}

void BaseState::startDial()
{
    logger.logError("Unexpected: startDial");
}

void BaseState::sendCallRequest(common::PhoneNumber number)
{
    logger.logError("Unexpected: sendCallRequest");
}

void BaseState::handleCallAccepted(common::PhoneNumber from)
{
    logger.logError("Unexpected: handleCallAccepted");
}

void BaseState::cancelCallRequest()
{
    logger.logError("Unexpected: cancelCallRequest");
}

void BaseState::handleCallRecipientNotAvailable()
{
    logger.logError("Unexpected: handleCallRecipientNotAvailable");
}

void BaseState::handleCallDropped()
{
    logger.logError("Unexpected: handleCallDropped");
}

void BaseState::handleRedirect()
{
    logger.logError("Unexpected: handleRedirect");
}

void BaseState::sendTalkMessage(const std::string& text)
{
    logger.logError("Unexpected: sendTalkMessage");
}

void BaseState::handleCallRequest(common::PhoneNumber)
{
    logger.logError("Unexpected: handleCallRequest");
}

void BaseState::callAccept(common::PhoneNumber)
{
    logger.logError("Unexpected: callAccept");
}

void BaseState::callDrop()
{
    logger.logError("Unexpected: callDrop");
}

}