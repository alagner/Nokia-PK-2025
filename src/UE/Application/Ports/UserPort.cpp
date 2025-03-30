#include "UserPort.hpp"
#include "UeGui/IListViewMode.hpp"
#include "UeGui/ITextMode.hpp"
#include <string>

namespace ue
{

UserPort::UserPort(common::ILogger &logger, IUeGui &gui, common::PhoneNumber phoneNumber)
    : logger(logger, "[USER-PORT]"),
      gui(gui),
      phoneNumber(phoneNumber)
{}

void UserPort::start(IEventsHandler &handler)
{
    this->handler = &handler;
    gui.setTitle("Nokia " + common::to_string(phoneNumber));

    gui.setAcceptCallback(std::bind(&UserPort::acceptCallback, this));
    gui.setRejectCallback(std::bind(&UserPort::rejectCallback, this));
}

void UserPort::stop()
{
    handler = nullptr;

    gui.setAcceptCallback(nullptr);
    gui.setRejectCallback(nullptr);
}

void UserPort::showNotConnected()
{
    currentViewMode = details::VIEW_MODE_UNKNOWN;
    gui.showNotConnected();
}

void UserPort::showConnecting()
{
    currentViewMode = details::VIEW_MODE_UNKNOWN;
    gui.showConnecting();
}

void UserPort::showConnected()
{
    currentViewMode = details::VIEW_MODE_MAIN_MENU;
    logger.logInfo("Showing Main Menu");
    IUeGui::IListViewMode& menu = gui.setListViewMode();
    menu.clearSelectionList();
    menu.addSelectionListItem("Compose SMS", "Send a new text message");
    menu.addSelectionListItem("View SMS", "Read received messages");

    gui.showConnected(); 
}

void UserPort::showNewSms()
{
    logger.logInfo("New SMS notification");
    gui.showNewSms(true);
}

void UserPort::showSmsList(const std::vector<SmsMessage>& messages)
{
    currentViewMode = details::VIEW_MODE_SMS_LIST;
    logger.logInfo("Showing SMS List (Count: ", messages.size(), ")");
    IUeGui::IListViewMode& menu = gui.setListViewMode();
    menu.clearSelectionList();
    for (const auto& sms : messages)
    {
        std::string prefix = sms.isRead ? "  " : "* "; 
        std::string label = prefix + "From: " + common::to_string(sms.from);
        menu.addSelectionListItem(label, sms.text); 
    }
}

// New method to display a single SMS
void UserPort::showSmsView(const SmsMessage& message)
{
    currentViewMode = details::VIEW_MODE_SMS_VIEW;
    logger.logInfo("Showing SMS from: ", message.from);
    IUeGui::ITextMode& viewer = gui.setViewTextMode(); 
    std::string displayText = "From: " + common::to_string(message.from) + "\n\n" + message.text;
    viewer.setText(displayText);
}


void UserPort::showAlert(const std::string& title, const std::string& message)
{
    
    currentViewMode = details::VIEW_MODE_UNKNOWN; 
    logger.logInfo("Showing Alert: ", title);
    IUeGui::ITextMode& alerter = gui.setAlertMode();
    alerter.setText(title + "\n\n" + message);
}



void UserPort::acceptCallback()
{
    if (!handler) return;

    std::optional<std::size_t> selectedIndexOpt;
   
    if (currentViewMode == details::VIEW_MODE_MAIN_MENU || currentViewMode == details::VIEW_MODE_SMS_LIST)
    {
        
        IUeGui::IListViewMode& currentList = gui.setListViewMode(); // Or gui.getCurrentListViewMode();
        auto selectionPair = currentList.getCurrentItemIndex();
        if (selectionPair.first) {
            selectedIndexOpt = selectionPair.second;
        }
    }
     else if (currentViewMode == details::VIEW_MODE_SMS_VIEW) {
         
         logger.logDebug("Accept pressed in SMS view - ignoring");
         return;
     }
    
    logger.logDebug("UI Action (Accept), Mode: ", currentViewMode, ", Index: ", (selectedIndexOpt.has_value() ? std::to_string(selectedIndexOpt.value()) : "N/A"));
    handler->handleUiAction(selectedIndexOpt);
}


void UserPort::rejectCallback()
{
    if (!handler) return;
    logger.logDebug("UI Action (Reject/Back), Mode: ", currentViewMode);
    
    handler->handleUiBack();
}

}
