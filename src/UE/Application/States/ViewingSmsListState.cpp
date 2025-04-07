#include "ViewingSmsListState.hpp"
#include "ViewingSingleSmsState.hpp" // Include for state transition
#include "ConnectedState.hpp"       // Include for state transition back
#include "Application.hpp"          // Include Application for getSmsDb
#include <string>                   // For string manipulation (find, substr)
#include <charconv>                 // For from_chars
#include <system_error>             // For std::errc

namespace ue
{

ViewingSmsListState::ViewingSmsListState(Context& context)
    : BaseState(context, "ViewingSmsListState")
{
    context.user.displaySmsList(context.app.getSmsDb());
}

void ViewingSmsListState::handleUserAction(const std::string& id)
{
    logger.logDebug("Handling user action: ", id);
    const std::string selectPrefix = "sms.list.select.";
    if (id == "REJECT") // "Back" action from list view
    {
        logger.logInfo("User pressed Back from SMS list, returning to Connected state.");
        context.setState<ConnectedState>();
    }
    else if (id.rfind(selectPrefix, 0) == 0) // Check if id starts with prefix
    {
        std::string indexStr = id.substr(selectPrefix.length());
        std::size_t index;
        // Use std::from_chars for safe string-to-integer conversion
        auto [ptr, ec] = std::from_chars(indexStr.data(), indexStr.data() + indexStr.size(), index);

        if (ec == std::errc() && ptr == indexStr.data() + indexStr.size()) // Check if conversion was successful and consumed entire string
        {
             if (index < context.app.getSmsDb().size()) {
                 logger.logInfo("User selected SMS at index: ", index);
                 // Pass index to the next state
                 context.setState<ViewingSingleSmsState>(index);
             } else {
                 logger.logError("Selected SMS index ", index, " is out of bounds.");
                 // Optional: show error to user? For now, just log.
                 // Stay in current state, redisplay list
                 context.user.displaySmsList(context.app.getSmsDb());
             }
        }
        else
        {
            logger.logError("Could not parse index from action ID: ", id);
            context.user.displaySmsList(context.app.getSmsDb()); // Redisplay list
        }
    }
    else if (id == "sms.list.empty") // User selected the "empty" placeholder
    {
         logger.logInfo("User acknowledged empty SMS list, returning to Connected state.");
         context.setState<ConnectedState>();
    }
     else
    {
        // CORRECTED: Use logInfo instead of logWarning
        logger.logInfo("Ignoring unexpected user action in this state: ", id);
        // BaseState::handleUserAction(id); // Optionally call base to log as error
    }
}

} // namespace ue