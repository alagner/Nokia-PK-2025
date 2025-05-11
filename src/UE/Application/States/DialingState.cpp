#include "DialingState.hpp"
#include "ConnectedState.hpp"

namespace ue {

DialingState::DialingState(Context& context, common::PhoneNumber callee)
    : BaseState(context, "DialingState"), callee(callee)
{
    context.logger.logInfo("Dialing number: ", callee);
    context.user.showAlert("Calling", "Calling " + common::to_string(callee));
    context.bts.sendCallRequest(callee);
}

void DialingState::handleUiBack()
{
    context.logger.logInfo("Call cancelled by user");
    // context.bts.sendCallDrop(callee);
    context.setState<ConnectedState>();
}

void DialingState::handleDisconnected()
{
    context.logger.logInfo("Disconnected while dialing");
    context.setState<ConnectedState>();
}

}
