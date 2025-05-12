#include "OutgoingCallState.hpp"
#include "IncomingCallState.hpp"
#include "ConnectedState.hpp"

namespace ue
{

    OutgoingCallState::OutgoingCallState(Context& context, common::PhoneNumber to)
        : BaseState(context, "OutgoingCallState"), callee(to)
    {
        logger.logInfo("Dialing: ", callee);
        context.bts.sendCallRequest(callee);
        context.user.showAlert("Calling", "Dialing number:\n" + common::to_string(callee));
    }

    void OutgoingCallState::handleCallRequest(common::PhoneNumber from)
    {
        logger.logInfo("Received call request while dialing");
        context.setState<IncomingCallState>(from);
    }

    void OutgoingCallState::handleUiBack()
    {
        logger.logInfo("User cancelled outgoing call to: ", callee);

        context.setState<ConnectedState>();
    }

}