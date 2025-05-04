#include "ConnectedState.hpp"
#include "NotConnectedState.hpp"
#include <sstream>
namespace ue
{

ConnectedState::ConnectedState(Context &context)
    : BaseState(context, "ConnectedState")
{
    context.user.showConnected();
}

void ConnectedState::handleDisconnect() {
    context.setState<NotConnectedState>();
}
void ConnectedState::handleSms(common::PhoneNumber from, std::string text)
{
    logger.logInfo("Received SMS from: ", from, ", text: ", text);

    context.user.showNewSms();
}
}
