#include "TalkingState.hpp"
#include "ConnectedState.hpp"
#include "NotConnectedState.hpp"
#include "UeGui/ICallMode.hpp"
#include "Ports/UserPort.hpp"

namespace ue
{

TalkingState::TalkingState(Context &context, common::PhoneNumber phoneNumber)
    : BaseState(context, "TalkingState"),
      peerPhoneNumber(phoneNumber),
      lastActivityWasReceive(false),
      lastActivityTime(std::chrono::steady_clock::now())
{
    context.user.showCallView(phoneNumber);
    
    context.user.clearIncomingCallText();
    context.user.clearOutgoingCallText();
    
    startInactivityTimer();
}

TalkingState::~TalkingState()
{
    context.timer.stopTimer();
}

void TalkingState::handleDisconnect()
{
    logger.logInfo("Connection to BTS dropped during call with: ", peerPhoneNumber);
    context.setState<NotConnectedState>();
}

void TalkingState::handleCallTalk(common::PhoneNumber from, std::string text)
{
    if (from == peerPhoneNumber) {
        logger.logInfo("Received call message from: ", from, ", text: ", text);
        context.user.showCallMessage(text);
        
        lastActivityWasReceive = true;
        resetInactivityTimer();
        
        checkInactivity();
    } else {
        logger.logError("Received call message from unknown peer: ", from, ", expected: ", peerPhoneNumber);
    }
}

void TalkingState::handleCallDropped(common::PhoneNumber from)
{
    if (from == peerPhoneNumber) {
        logger.logInfo("Call dropped by peer: ", from);
        context.user.showPeerUserNotAvailable(from);
        logger.logInfo("Returning to ConnectedState");
        context.setState<ConnectedState>();
    } else {
        logger.logError("Received call dropped from unknown peer: ", from, ", expected: ", peerPhoneNumber);
    }
}

void TalkingState::handleCallRequest(common::PhoneNumber from)
{
    if (from == peerPhoneNumber) {
        logger.logInfo("Received subsequent call request from same peer: ", from, " - ignoring as already in call with this peer");
    } else {
        logger.logInfo("Received subsequent call request from: ", from, " while in call with: ", peerPhoneNumber, " - dropping new request");
        context.bts.sendCallDropped(from);
    }
}

void TalkingState::handleSms(common::PhoneNumber from, std::string text)
{
    logger.logInfo("Received SMS from: ", from, " while in call with: ", peerPhoneNumber);
    
    context.smsDb.addSms(from, text);
    
    context.user.showNewSms(true);
    
    logger.logInfo("SMS stored in background, call continues uninterrupted");
}

void TalkingState::handleUnknownRecipient(common::PhoneNumber to)
{
    if (to == peerPhoneNumber) {
        logger.logInfo("Recipient not available: ", to);
        context.user.showPeerUserNotAvailable(to);
        context.setState<ConnectedState>();
    }
}

void TalkingState::acceptCallRequest()
{
    logger.logInfo("Sending message in talking state");
    
    std::string message = context.user.getOutgoingCallText();
    
    if (!message.empty()) {
        sendCallMessage(message);
        context.user.clearOutgoingCallText();
    } else {
        logger.logInfo("No message to send - text field is empty");
    }
}

void TalkingState::rejectDialing()
{
    logger.logInfo("User ended the call with: ", peerPhoneNumber);
    
    context.bts.sendCallDropped(peerPhoneNumber);
    
    context.setState<ConnectedState>();
}

void TalkingState::handleTimeout()
{
    logger.logInfo("Timer event received, checking for inactivity");
    checkInactivity();
}

void TalkingState::sendCallMessage(const std::string& message)
{
    logger.logInfo("Sending call message to: ", peerPhoneNumber, ", text: ", message);
    
    context.bts.sendCallTalk(peerPhoneNumber, message);
    
    context.user.showCallMessage("You: " + message);
    
    lastActivityWasReceive = false;
    resetInactivityTimer();
    
    checkInactivity();
}

void TalkingState::startInactivityTimer()
{
    context.timer.startTimer(std::chrono::seconds(30));
}

void TalkingState::resetInactivityTimer()
{
    lastActivityTime = std::chrono::steady_clock::now();
}

void TalkingState::checkInactivity()
{
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::minutes>(now - lastActivityTime);
    
    if (elapsed >= INACTIVITY_TIMEOUT) {
        logger.logInfo("Call timeout due to inactivity for ", elapsed.count(), " minutes");
        
        if (!lastActivityWasReceive) {
            context.bts.sendCallDropped(peerPhoneNumber);
        }
        
        context.setState<ConnectedState>();
    } else {
        logger.logDebug("Inactivity check: ", elapsed.count(), " minutes elapsed of ", 
                       INACTIVITY_TIMEOUT.count(), " minutes timeout");
        startInactivityTimer();
    }
}

void TalkingState::rejectCallRequest()
{
    logger.logInfo("User hung up the call with: ", peerPhoneNumber);
    
    context.bts.sendCallDropped(peerPhoneNumber);
    
    context.setState<ConnectedState>();
}

void TalkingState::handleClose()
{
    logger.logInfo("User closes UE while in call with: ", peerPhoneNumber);
    
    logger.logInfo("Sending CallDropped to peer before closing");
    context.bts.sendCallDropped(peerPhoneNumber);
    
    logger.logInfo("UE closing immediately without waiting for call completion");
}

}
