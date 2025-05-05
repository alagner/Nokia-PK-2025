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

}
