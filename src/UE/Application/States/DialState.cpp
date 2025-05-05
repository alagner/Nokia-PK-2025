#include "DialState.hpp"
#include "ConnectedState.hpp"
#include "TalkingState.hpp"

namespace ue
{

const std::chrono::seconds DialState::CALL_REQUEST_TIMEOUT{60}; // 60 seconds timeout as per requirements

DialState::DialState(Context &context)
    : BaseState(context, "DialState")
{
    context.user.showDialView();
}

void DialState::handleTimeout()
{
    logger.logInfo("Call request timeout - call dropped");
    context.user.showPeerUserNotAvailable(recipientPhoneNumber);
    context.setState<ConnectedState>();
}

void DialState::acceptDialing(common::PhoneNumber recipientNumber)
{
    logger.logInfo("Sending call request to: ", recipientNumber);
    
    recipientPhoneNumber = recipientNumber;
    
    context.timer.startTimer(CALL_REQUEST_TIMEOUT);
    
    logger.logInfo("About to send CallRequest message to recipient: ", recipientPhoneNumber);
    
    context.bts.sendCallRequest(recipientPhoneNumber);
    
    logger.logInfo("CallRequest message sent, waiting for response...");
}

void DialState::rejectDialing()
{
    logger.logInfo("User canceled call request");
    

    context.timer.stopTimer();
    
    if (recipientPhoneNumber != common::PhoneNumber{}) {
        context.bts.sendCallDropped(recipientPhoneNumber);
    }
    
    context.setState<ConnectedState>();
}

void DialState::handleCallAccepted(common::PhoneNumber from)
{
    logger.logInfo("Received CallAccepted message from: ", from);
    
    if (from == recipientPhoneNumber) {
        logger.logInfo("Call accepted by: ", from);
        
        context.timer.stopTimer();
        
        logger.logInfo("Changing to TalkingState with peer: ", recipientPhoneNumber);
        context.setState<TalkingState>(recipientPhoneNumber);
    } else {
        logger.logError("Call accepted from unknown number: ", from, " but expected: ", recipientPhoneNumber);
    }
}

void DialState::handleCallDropped(common::PhoneNumber from)
{
    if (from == recipientPhoneNumber) {
        logger.logInfo("Call dropped by: ", from);
        
        context.timer.stopTimer();
        
        context.user.showPeerUserNotAvailable(from);
        context.setState<ConnectedState>();
    } else {
        logger.logError("Call dropped from unknown number: ", from);
    }
}

}