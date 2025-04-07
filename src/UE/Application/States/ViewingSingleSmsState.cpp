#include "ViewingSingleSmsState.hpp"
#include "ViewingSmsListState.hpp" // Include for state transition back
#include "Application.hpp"         // Include Application for getSmsDb/markSmsAsRead/storeReceivedSms

namespace ue
{

ViewingSingleSmsState::ViewingSingleSmsState(Context& context, std::size_t smsIndex)
    : BaseState(context, "ViewingSingleSmsState"),
      smsIndex(smsIndex)
{
    const auto& smsDb = context.app.getSmsDb();
    if (smsIndex < smsDb.size())
    {
        logger.logInfo("Displaying SMS content for index: ", smsIndex);
        const auto& sms = smsDb[smsIndex];
        context.user.viewSms(sms);
        // Mark as read after displaying
        if (!sms.isRead) {
             context.app.markSmsAsRead(smsIndex);
        }
    }
    else
    {
        logger.logError("Invalid SMS index provided: ", smsIndex);
        context.setState<ViewingSmsListState>();
    }
}

void ViewingSingleSmsState::handleUserAction(const std::string& id)
{
    logger.logDebug("Handling user action: ", id);
    if (id == "REJECT") // "Back" action from SMS view
    {
        logger.logInfo("User pressed Back from SMS view, returning to list.");
        context.setState<ViewingSmsListState>();
    }
    else
    {
        logger.logInfo("Ignoring unexpected user action in this state: ", id);
    }
}

// Add handleSms override implementation
void ViewingSingleSmsState::handleSms(const common::PhoneNumber& from, const std::string& text)
{
    logger.logInfo("Received SMS from: ", from, " while viewing single SMS (storing in background).");
    // Store the SMS using the application method
    context.app.storeReceivedSms(from, text);
    // updateSmsIndicator() is called automatically by storeReceivedSms
}


}