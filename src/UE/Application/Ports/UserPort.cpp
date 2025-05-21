#include "UserPort.hpp"
#include "UeGui/IListViewMode.hpp"
#include "UeGui/ICallMode.hpp"
#include "UeGui/IDialMode.hpp"

namespace ue
{

UserPort::UserPort(common::ILogger &logger, IUeGui &gui, common::PhoneNumber phoneNumber)
    : logger(logger, "[USER-PORT]"),
      gui(gui),
      phoneNumber(phoneNumber)
{}

void UserPort::start(IUserEventsHandler &handler)
{
    this->handler = &handler;
    gui.setTitle("Nokia " + to_string(phoneNumber));
}

void UserPort::stop()
{
    handler = nullptr;
}

void UserPort::showNotConnected()
{
    gui.showNotConnected();
}

void UserPort::showConnecting()
{
    gui.showConnecting();
}

void UserPort::showConnected()
{
    IUeGui::IListViewMode& menu = gui.setListViewMode();
    menu.clearSelectionList();
    menu.addSelectionListItem("Compose SMS", "");
    menu.addSelectionListItem("View SMS", "");
    menu.addSelectionListItem("Dial", "");  
    
    gui.setAcceptCallback([this]() {
        auto selection = gui.setListViewMode().getCurrentItemIndex();
        if (selection.first && selection.second == 2) {  // Check if "Dial" (index 2) was selected
            handleDialClicked();
        }
    });
}

void UserPort::showNewSms(bool present)
{
    logger.log(common::ILogger::INFO_LEVEL, "New SMS received!");
    gui.showNewSms(present);
}

void UserPort::showCallRequest(common::PhoneNumber phoneNumber)
{
    logger.logInfo("Showing call request from: ", phoneNumber);
    IUeGui::ICallMode& callMode = gui.setCallMode();
    
    callMode.clearIncomingText();
    callMode.appendIncomingText("Incoming call from " + to_string(phoneNumber));
    
    gui.setAcceptCallback([this]() { handleAcceptCallClicked(); });
    gui.setRejectCallback([this]() { handleRejectCallClicked(); });
}

void UserPort::showCallView(common::PhoneNumber phoneNumber)
{
    logger.logInfo("Showing call view with: ", phoneNumber);
    IUeGui::ICallMode& callMode = gui.setCallMode();
    
    callMode.clearIncomingText();
    callMode.appendIncomingText("Call with " + to_string(phoneNumber));

    gui.setAcceptCallback([this]() { handleAcceptCallClicked(); });
    gui.setRejectCallback([this]() { handleRejectCallClicked(); });
}

void UserPort::showCallMessage(std::string message)
{
    logger.logInfo("Showing call message: ", message);
    IUeGui::ICallMode& callMode = gui.setCallMode();
    callMode.appendIncomingText(message);
}

void UserPort::showPeerUserNotAvailable(common::PhoneNumber phoneNumber)
{
    logger.logInfo("Showing peer not available for: ", phoneNumber);
    gui.showPeerUserNotAvailable(phoneNumber);
}

void UserPort::showDialView()
{
    logger.logInfo("Showing dial view");
    IUeGui::IDialMode& dialMode = gui.setDialMode();
    
    gui.setAcceptCallback([this]() { handleAcceptDialingClicked(); });
    gui.setRejectCallback([this]() { handleRejectDialingClicked(); });
}

void UserPort::handleAcceptCallClicked()
{
    logger.logInfo("User clicked accept call");
    if (handler)
    {
        handler->acceptCallRequest();
    }
}

void UserPort::handleRejectCallClicked()
{
    logger.logInfo("User clicked reject call");
    if (handler)
    {
        handler->rejectCallRequest();
    }
}

void UserPort::handleDialClicked()
{
    logger.logInfo("User clicked dial");
    if (handler)
    {
        handler->dial();
    }
}

void UserPort::handleAcceptDialingClicked()
{
    logger.logInfo("User clicked accept dialing");
    if (handler)
    {
        IUeGui::IDialMode& dialMode = gui.setDialMode();
        common::PhoneNumber recipientNumber = dialMode.getPhoneNumber();
        handler->acceptDialing(recipientNumber);
    }
}

void UserPort::handleRejectDialingClicked()
{
    logger.logInfo("User clicked reject dialing");
    if (handler)
    {
        handler->rejectDialing();
    }
}

<<<<<<< Updated upstream
=======
void UserPort::handleViewSmsClicked()
{
    logger.logInfo("User clicked view SMS");
    if (handler)
    {
        handler->viewSms();
    }
}

void UserPort::handleSelectSmsClicked(size_t index)
{
    logger.logInfo("User selected SMS at index: ", index);
    if (handler)
    {
        handler->selectSms(index);
    }
}

void UserPort::handleCloseSmsViewClicked()
{
    logger.logInfo("User closed SMS view");
    if (handler)
    {
        handler->closeSmsView();
    }
}

void UserPort::handleComposeSmsClicked()
{
    logger.logInfo("User clicked compose SMS");
    if (handler)
    {
        handler->composeSms();
    }
}

void UserPort::handleAcceptSmsComposeClicked()
{
    logger.logInfo("User accepted SMS composition");
    if (handler)
    {
        IUeGui::ISmsComposeMode& smsComposeMode = gui.setSmsComposeMode();
        common::PhoneNumber recipientNumber = smsComposeMode.getPhoneNumber();
        std::string smsText = smsComposeMode.getSmsText();
        
        smsComposeMode.clearSmsText();
        
        handler->acceptSmsCompose(recipientNumber, smsText);
    }
}

void UserPort::handleRejectSmsComposeClicked()
{
    logger.logInfo("User rejected SMS composition");
    if (handler)
    {
        handler->rejectSmsCompose();
    }
}

IUeGui::ICallMode& UserPort::getCallMode()
{
    logger.logDebug("Getting call mode UI");
    return gui.setCallMode();
}

std::string UserPort::getOutgoingCallText()
{
    logger.logDebug("Getting outgoing call text");
    return gui.setCallMode().getOutgoingText();
}

void UserPort::clearOutgoingCallText()
{
    logger.logDebug("Clearing outgoing call text");
    gui.setCallMode().clearOutgoingText();
}

void UserPort::clearIncomingCallText()
{
    logger.logDebug("Clearing incoming call text");
    gui.setCallMode().clearIncomingText();
}

>>>>>>> Stashed changes
}
