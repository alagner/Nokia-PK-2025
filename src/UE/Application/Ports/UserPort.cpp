#include "UserPort.hpp"
#include "Application.hpp" // Include Application to access handler
#include "UeGui/IListViewMode.hpp"
#include "UeGui/ITextMode.hpp" // Include for setViewTextMode
#include "IUeGui.hpp" // Include IUeGui for showNewSms etc.
#include <sstream> // Include for formatting SMS list items
#include <vector>  // Include for vector used in displaySmsList

namespace ue
{

UserPort::UserPort(common::ILogger &logger, IUeGui &gui, common::PhoneNumber phoneNumber)
    : logger(logger, "[USER-PORT]"),
      gui(gui),
      phoneNumber(phoneNumber)
{}

void UserPort::start(IUserEventsHandler &handler_param)
{
    this->handler = &handler_param; // Assign parameter address to the member variable
    gui.setTitle("Nokia " + common::to_string(phoneNumber));

    // Set only the generic Reject callback here (likely used for "Back" or "Cancel")
    gui.setRejectCallback([this]() {
         if(this->handler) { // Access member via this->
             logger.logDebug("Reject callback triggered");
             this->handler->handleUserAction("REJECT"); // Or maybe "BACK" depending on context?
        } else {
             logger.logError("Reject callback triggered, but handler is null");
         }
    });
    // Accept callback will be set contextually by modes that need it (like list views)
    gui.setAcceptCallback(nullptr); // Clear any previous accept callback initially
}

void UserPort::stop()
{
    this->handler = nullptr; // Set handler to null
}

void UserPort::showNotConnected()
{
    gui.setAcceptCallback(nullptr); // No accept action
    gui.showNotConnected();
}

void UserPort::showConnecting()
{
    gui.setAcceptCallback(nullptr); // No accept action
    gui.showConnecting();
}

void UserPort::showConnected()
{
    logger.logInfo("Showing Connected state main menu");
    IUeGui::IListViewMode& menu = gui.setListViewMode();
    menu.clearSelectionList();
    gui.setTitle("Nokia " + common::to_string(phoneNumber)); // Reset title for main menu

    // Define items and their corresponding action IDs
    const std::vector<std::pair<std::string, std::string>> items = {
        {"Compose SMS", "sms.compose"},
        {"View SMS", "sms.view"},
        {"Dial Call", "call.dial"}
    };

    for(const auto& item : items) {
        menu.addSelectionListItem(item.first, item.second); // Label and action ID
    }

    // Set the Accept callback specifically for this menu
    gui.setAcceptCallback([this, &menu, items](){
        IUeGui::IListViewMode::OptionalSelection selection = menu.getCurrentItemIndex();
        if (selection.first && selection.second < items.size()) {
            const std::string& actionId = items[selection.second].second;
            logger.logDebug("Main menu Accept: item ", selection.second, ", action: ", actionId);
            if (this->handler) {
                this->handler->handleUserAction(actionId);
            } else {
                 logger.logError("Accept callback (main menu) triggered, but handler is null");
            }
        } else {
             // CORRECTED: Use logInfo instead of logWarning
             logger.logInfo("Accept callback (main menu) triggered, but no valid item selected.");
        }
    });

    // Explicitly hide SMS indicator when showing main menu
    gui.showNewSms(false);
}

void UserPort::showNewSms(bool present)
{
    logger.logInfo("Setting new SMS indicator: ", (present ? "ON" : "OFF"));
    gui.showNewSms(present);
}

void UserPort::displaySmsList(const std::vector<data::SmsData>& smsList)
{
    logger.logInfo("Displaying SMS list with ", smsList.size(), " items.");
    IUeGui::IListViewMode& menu = gui.setListViewMode();
    menu.clearSelectionList();
    gui.setTitle("Received SMS"); // Set the overall GUI title

    std::size_t listSize = smsList.size(); // For capture

    if (smsList.empty()) {
         menu.addSelectionListItem("No SMS messages", "sms.list.empty");
         gui.setAcceptCallback(nullptr); // No accept action if list is empty
    } else {
        for (std::size_t i = 0; i < listSize; ++i) {
            const auto& sms = smsList[i];
            std::stringstream ss;
            ss << (sms.isRead ? "  " : "[N] ") << "From: " << common::to_string(sms.from);
            // Add item with label. We'll use index in callback. Tooltip not used here.
            menu.addSelectionListItem(ss.str(), "");
        }

        // Set the Accept callback specifically for the SMS list
        gui.setAcceptCallback([this, &menu, listSize](){
            IUeGui::IListViewMode::OptionalSelection selection = menu.getCurrentItemIndex();
            if (selection.first && selection.second < listSize) {
                std::string actionId = "sms.list.select." + std::to_string(selection.second);
                logger.logDebug("SMS List Accept: item ", selection.second, ", action: ", actionId);
                 if (this->handler) {
                    this->handler->handleUserAction(actionId);
                } else {
                    logger.logError("Accept callback (SMS list) triggered, but handler is null");
                }
            } else {
                // CORRECTED: Use logInfo instead of logWarning
                logger.logInfo("Accept callback (SMS list) triggered, but no valid item selected.");
            }
        });
    }
     // Rely on generic RejectCallback set in start() for "Back" functionality
}

void UserPort::viewSms(const data::SmsData& sms)
{
    logger.logInfo("Viewing SMS from: ", sms.from);
    IUeGui::ITextMode& view = gui.setViewTextMode();
    gui.setTitle("View SMS"); // Set the overall GUI title
    std::stringstream ss;
    ss << "From: " << common::to_string(sms.from) << "\n\n";
    ss << sms.text;
    view.setText(ss.str());
    // Accept/Reject callbacks should handle going "Back" (Reject likely)
    // Setting Accept callback to null if no action needed on green button press
    gui.setAcceptCallback(nullptr);
}

// This handler is not needed anymore as selection is handled via AcceptCallback
// void UserPort::handleMenuSelection(const std::string& id) { ... }


} // namespace ue