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
    context.logger.log(common::ILogger::INFO_LEVEL, "[ConnectedState] Received SMS from: ", from, ", text: ", text);

    context.user.showNewSms();
    context.user.addReceivedSms(from, text);
}

void ConnectedState::handleDisconnect()
{
    context.setState<NotConnectedState>();
}

}
