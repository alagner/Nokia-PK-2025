#include "ConnectedState.hpp"
#include <sstream>
#include "NotConnectedState.hpp"
#include "TalkingState.h"

namespace ue
{

ConnectedState::ConnectedState(Context &context)
    : BaseState(context, "ConnectedState")
{
    context.user.showConnected();
}
void ConnectedState::handleSms(common::PhoneNumber from, std::string text)
{
    logger.logInfo(common::ILogger::INFO_LEVEL, "[ConnectedState] Received SMS from: ", from, ", text: ", text);
    context.user.showNewSms();
}

void ConnectedState::viewSmsList()
{
    auto smsList = context.smsDb.getAll();
    context.user.showSmsList(smsList);
}

void ConnectedState::viewSms(unsigned int index)
{
    auto smsList = context.smsDb.getAll();
    auto chosenSms = smsList.at(index);
    if (!smsList[index].isRead){
        smsList[index].isRead = true;
        context.smsDb.saveAll(smsList);
    }
    context.user.showSms(chosenSms);
}

void ConnectedState::handleDisconnect()
{
    context.setState<NotConnectedState>();
}

void ConnectedState::sendSms(const SmsEntity& sms)
{
    auto fromPhoneNumber = context.user.getPhoneNumber().value;
    auto toPhoneNumber = sms.to;
    auto messageText = sms.text;

    SmsEntity sentSms(fromPhoneNumber, toPhoneNumber, messageText, false);
    context.smsDb.save(sentSms);
    context.user.showConnected();
    context.bts.sendSms(sentSms);
}

void ConnectedState::composeSms()
{
    logger.logInfo("Composing new SMS");
    context.user.composeSms();
}

void ConnectedState::handleSmsDeliveryFailure(common::PhoneNumber from)
{
    logger.logInfo("Received UnknownRecipient for SMS from: ", from);
    auto smsList = context.smsDb.getAll();
    if (smsList.empty()) return;

    for (auto sms = smsList.rbegin(); sms != smsList.rend(); ++sms){
        if(sms->from==from.value){
            sms->text = "[FAILED DELIVERY] \n" + sms->text;
            context.smsDb.saveAll(smsList, true);
            logger.logInfo("Marked SMS from ", from, " as failed.");
            break;
        }
    }

}

void ConnectedState::startDial()
{
    context.user.startDial();
}

void ConnectedState::sendCallRequest(common::PhoneNumber number)
{
    callTarget = number;
    auto myNumber = context.user.getPhoneNumber();
    context.bts.sendCallRequest(myNumber, number);
    context.timer.startTimer(std::chrono::seconds(60));
    context.user.showDialing();
}

void ConnectedState::handleCallAccepted()
{
    context.timer.stopTimer();
    context.setState<TalkingState>();
}

void ConnectedState::handleCallDropped()
{
    logger.logInfo("Call dropped by peer.");
    context.user.showConnected();
    context.timer.stopTimer();
}


void ConnectedState::cancelCallRequest()
{
    context.timer.stopTimer();
    context.bts.sendCallDropped(context.user.getPhoneNumber(), callTarget);
    context.user.showConnected();
}

void ConnectedState::handleCallRecipientNotAvailable(common::PhoneNumber from)
{
    logger.logInfo("Recipient not available for call from ", from);
    context.timer.stopTimer();
    context.user.showPartnerNotAvailable();
    context.timer.startRedirectTimer(std::chrono::seconds(3));
}

void ConnectedState::handleTimeout()
{
    logger.logInfo("Call timeout – recipient did not answer.");
    context.bts.sendCallDropped(context.user.getPhoneNumber(), callTarget);
    context.user.showConnected();
}

void ConnectedState::handleRedirect()
{
    context.user.showConnected();
}

}
