#include "TalkingState.h"
#include "NotConnectedState.hpp"

namespace ue {

TalkingState::TalkingState(Context& context)
    : BaseState(context, "TalkingState")
{
    context.user.showTalking();
}

void TalkingState::handleDisconnect()
{
    logger.logInfo("Disconnected during call");
    context.setState<NotConnectedState>();
}

}
