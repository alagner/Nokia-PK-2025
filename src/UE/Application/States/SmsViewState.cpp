#include "SmsViewState.hpp"
#include "ConnectedState.hpp"
#include "TalkingState.hpp"
#include "../Ports/ISmsListViewPort.hpp"
#include <sstream>
#include <chrono>
#include <thread>

namespace ue
{

const std::chrono::seconds SmsViewState::CALL_TIMEOUT{30};

SmsViewState::SmsViewState(Context &context)
    : BaseState(context, "SmsViewState")
{
    logger.logInfo("Entered SMS view state");
    
    
    auto* smsListViewPort = dynamic_cast<ISmsListViewPort*>(&context.user);
    if (smsListViewPort) {
        smsListViewPort->setSmsList(context.smsDb.getAllSms());
        
        smsListViewPort->setSelectSmsCallback([this](size_t index) {
            this->selectSms(index);
        });
    } else {
        logger.logError("Failed to cast UserPort to ISmsListViewPort");
    }
    
    context.user.showSmsList();
    
   
    updateNotificationIcon("Entering SmsViewState");
}

void SmsViewState::handleDisconnect() 
{
    context.setState<ConnectedState>();
    
}

void SmsViewState::handleSms(common::PhoneNumber from, std::string text)
{
    logger.logInfo("Received SMS from: ", from, ", text: ", text);
    
    context.smsDb.addSms(from, text);
    
    
    context.user.showNewSms(true);
    
    
    viewSms();
}

void SmsViewState::viewSms()
{
    logger.logInfo("Refreshing SMS list view");
    
    updateNotificationIcon("viewSms");

    auto* smsListViewPort = dynamic_cast<ISmsListViewPort*>(&context.user);
    if (smsListViewPort) {
        smsListViewPort->setSmsList(context.smsDb.getAllSms());
        
        smsListViewPort->setSelectSmsCallback([this](size_t index) {
            this->selectSms(index);
        });
    } else {
        logger.logError("Failed to cast UserPort to ISmsListViewPort");
    }
    
    context.user.showSmsList();
}

void SmsViewState::selectSms(size_t index)
{
    logger.logInfo("User selected SMS at index: ", index);
    
    const auto& smsList = context.smsDb.getAllSms();
    
    if (index < smsList.size()) {
        const auto& sms = smsList[index];
        
        if (sms.isSent) {
            context.user.showSentSmsContent(to_string(sms.to), sms.text);
        } else {
            context.user.showSmsContent(to_string(sms.from), sms.text);
        }
        
        if (!sms.isRead) {
            context.smsDb.markAsRead(index);
            
            updateNotificationIcon("selectSms");
        }
    } else {
        logger.logError("Invalid SMS index: ", index);
        viewSms();
    }
}

void SmsViewState::closeSmsView()
{
    logger.logInfo("User closed SMS view");
    
  
    context.setState<ConnectedState>();
}



void SmsViewState::handleCallRequest(common::PhoneNumber from)
{
    logger.logInfo("Received call request from: ", from);
    callingPhoneNumber = from;
    
    context.timer.startTimer(CALL_TIMEOUT);
    
    logger.logInfo("Showing call request UI for caller: ", from);
    context.user.showCallRequest(from);
}

void SmsViewState::acceptCallRequest()
{
    logger.logInfo("User accepted call from: ", callingPhoneNumber);
    
    context.timer.stopTimer();
    
    logger.logInfo("Sending CallAccept message to: ", callingPhoneNumber);
    context.bts.sendCallAccept(callingPhoneNumber);
    
    logger.logInfo("Changing to TalkingState with peer: ", callingPhoneNumber);
    context.setState<TalkingState>(callingPhoneNumber);
}

void SmsViewState::rejectCallRequest()
{
    logger.logInfo("User rejected call from: ", callingPhoneNumber);
    
    context.timer.stopTimer();
    
    context.bts.sendCallDropped(callingPhoneNumber);
    
   
    viewSms();
}

void SmsViewState::handleTimeout()
{
    logger.logInfo("Call request timeout - auto-rejecting call from: ", callingPhoneNumber);
    
    context.bts.sendCallDropped(callingPhoneNumber);
    
   
    viewSms();
}

void SmsViewState::updateNotificationIcon(const std::string& source)
{
    bool hasUnread = context.smsDb.hasUnreadSms();
    logger.logInfo("Updating SMS notification icon (", source, "): ", (hasUnread ? "visible" : "hidden"));
    
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    
    context.user.showNewSms(hasUnread);
}

}
