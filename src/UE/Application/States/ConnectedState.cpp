#include "ConnectedState.hpp"
#include "NotConnectedState.hpp"
#include "TalkingState.hpp"
#include "DialState.hpp"
<<<<<<< Updated upstream
=======
#include "../Ports/ISmsListViewPort.hpp"
#include "../Ports/UserPort.hpp"
#include "UeGui/ICallMode.hpp"
>>>>>>> Stashed changes
#include <sstream>
namespace ue
{

const std::chrono::seconds ConnectedState::CALL_TIMEOUT{30}; // 30 seconds timeout as per requirements

ConnectedState::ConnectedState(Context &context)
    : BaseState(context, "ConnectedState")
{
    try {
        context.user.clearIncomingCallText();
        context.user.clearOutgoingCallText();
    } catch (const std::bad_cast&) {
        logger.logError("Failed to cast IUserPort to UserPort to clear call history");
    }
    
    context.user.showConnected();
}

void ConnectedState::handleDisconnect() {
    context.setState<NotConnectedState>();
}

void ConnectedState::handleSms(common::PhoneNumber from, std::string text)
{
    logger.logInfo("Received SMS from: ", from, ", text: ", text);

    context.user.showNewSms(true);
}

void ConnectedState::handleCallRequest(common::PhoneNumber from)
{
    logger.logInfo("Received call request from: ", from);
    callingPhoneNumber = from;
    
    context.timer.startTimer(CALL_TIMEOUT);
    
    logger.logInfo("Showing call request UI for caller: ", from);
    context.user.showCallRequest(from);
}

void ConnectedState::handleTimeout()
{
    logger.logInfo("Call request timeout - auto-rejecting call from: ", callingPhoneNumber);
    
    context.bts.sendCallDropped(callingPhoneNumber);
    
    context.user.showConnected();
}

void ConnectedState::acceptCallRequest()
{
    logger.logInfo("User accepted call from: ", callingPhoneNumber);
    
    context.timer.stopTimer();
    
    logger.logInfo("Sending CallAccept message to: ", callingPhoneNumber);
    context.bts.sendCallAccept(callingPhoneNumber);
    
    logger.logInfo("Changing to TalkingState with peer: ", callingPhoneNumber);
    context.setState<TalkingState>(callingPhoneNumber);
}

void ConnectedState::rejectCallRequest()
{
    logger.logInfo("User rejected call from: ", callingPhoneNumber);
    
    context.timer.stopTimer();
    
    context.bts.sendCallDropped(callingPhoneNumber);
    
    context.user.showConnected();
}

void ConnectedState::dial()
{
    logger.logInfo("User initiated dialing");
    
    context.setState<DialState>();
}

}
