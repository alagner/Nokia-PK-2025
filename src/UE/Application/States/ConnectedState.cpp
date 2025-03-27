#include "ConnectedState.hpp"
#include "NotConnectedState.hpp"

namespace ue
{

    ConnectedState::ConnectedState(Context &context)
        : BaseState(context, "ConnectedState")
    {
        logger.logInfo("Connected state entered");
        context.user.showConnected(); // Signal to the user that UE is connected
    }

    void ConnectedState::handleDisconnected()
    {
        logger.logInfo("Connection to BTS lost");
        context.setState<NotConnectedState>();
    }

}