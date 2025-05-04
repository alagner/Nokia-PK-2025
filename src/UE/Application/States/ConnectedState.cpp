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

void ConnectedState::handleDisconnect()
{
    context.setState<NotConnectedState>();
}

}
