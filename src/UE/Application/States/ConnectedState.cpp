#include "ConnectedState.hpp"
#include "NotConnectedState.hpp"
#include "ViewingSmsListState.hpp" 
#include <vector> // Include vector

namespace ue
{
    // Constructor calls showMainMenu now
    ConnectedState::ConnectedState(Context &context)
        : BaseState(context, "ConnectedState")
    {
        showMainMenu();
    }

    void ConnectedState::showMainMenu()
    {
        logger.logInfo("Entering Main Menu");
        context.user.showConnected(); // This UserPort method now shows the menu
    }

    void ConnectedState::handleDisconnected()
    {
        logger.logInfo("Connection to BTS lost");
        context.setState<NotConnectedState>();
    }

    void ConnectedState::handleSmsReceived(common::PhoneNumber from, std::string text)
    {
        logger.logInfo("SMS received from: ", from); // Log removed text for brevity
        std::size_t smsIndex = context.smsDb.addSms(from, text);
        logger.logDebug("SMS stored at index: ", smsIndex);
        context.user.showNewSms(); // Show notification, but stay in main menu
    }

    // Handle selection from the main menu list
    void ConnectedState::handleUiAction(std::optional<std::size_t> selectedIndex)
    {
        if (!selectedIndex.has_value()) {
            logger.logInfo("UI Action received with no index in Main Menu");
            return;
        }

        std::size_t index = selectedIndex.value();
        logger.logInfo("Main menu action selected: index ", index);

        // Assuming order: 0: Compose SMS, 1: View SMS
        if (index == 0)
        {
            logger.logInfo("Compose SMS action selected - (Not Implemented Yet)");
            // context.setState<ComposingSmsState>(); // Transition to compose state
        }
        else if (index == 1)
        {
            logger.logInfo("View SMS action selected");
            context.setState<ViewingSmsListState>(); // Transition to SMS list state
        }
        else
        {
            logger.logInfo("Unknown main menu index selected: ", index);
        }
    }

    void ConnectedState::handleUiBack()
    {
        // What should back do from the main menu? Nothing? Exit?
        logger.logInfo("Back action in main menu - ignored");
    }

} // namespace ue