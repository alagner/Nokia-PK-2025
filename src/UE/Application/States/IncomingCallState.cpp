#include "IncomingCallState.hpp"
#include "ConnectedState.hpp"

namespace ue
{

    IncomingCallState::IncomingCallState(Context& context, common::PhoneNumber from)
        : BaseState(context, "IncomingCallState"), caller(from)
    {
        context.user.showIncomingCall(from);
    }

    void IncomingCallState::handleUiAction(std::optional<std::size_t>)
    {
        logger.logInfo("Call accepted from: ", caller);
        // TODO CallAccept
        context.setState<ConnectedState>();
    }

    void IncomingCallState::handleUiBack()
    {
        logger.logInfo("Call rejected from: ", caller);
        // TODO: CallDrop
        context.setState<ConnectedState>();
    }

}