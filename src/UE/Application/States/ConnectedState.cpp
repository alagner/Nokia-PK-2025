#include "ConnectedState.hpp"
#include <sstream>
#include "NotConnectedState.hpp"
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

}
