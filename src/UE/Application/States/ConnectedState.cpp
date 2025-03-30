#include "ConnectedState.hpp"
#include "NotConnectedState.hpp"
#include "ViewingSmsListState.hpp" 
#include "ComposingSmsState.hpp"
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
        logger.logInfo("SMS received from: ", from);
        // Use the correct add method
        std::size_t smsIndex = context.smsDb.addReceivedSms(from, text);
        logger.logDebug("SMS stored at index: ", smsIndex);
        context.user.showNewSms();
    }

    // Handle selection from the main menu list
    void ConnectedState::handleUiAction(std::optional<std::size_t> selectedIndex)
    {
        if (!selectedIndex.has_value()) {
            logger.logInfo("UI Action received with no index in Main Menu");
            return;
        }

        logger.logInfo("Main menu selection: index=", selectedIndex.value());
    
        switch (selectedIndex.value())
        {
        case 0: // "Compose SMS"
            logger.logInfo("Compose SMS selected");
            context.setState<ComposingSmsState>();
            break;
            
        case 1: // "View SMS"
            logger.logInfo("View SMS selected");
            context.setState<ViewingSmsListState>();
            break;
            
        default:
            logger.logError("Invalid menu option selected: ", selectedIndex.value());
            break;
        }
    }

    void ConnectedState::handleUiBack()
    {
         logger.logInfo("Back action in main menu - ignored");
    }

    void ConnectedState::handleSmsSentResult(common::PhoneNumber to, bool success)
    {
        logger.logInfo("Received SMS send result for ", to, " while in main menu. Success: ", success);
        if (!success) {
            if (!context.smsDb.markLastOutgoingSmsAsFailed()) {
                logger.logInfo("Could not mark last outgoing SMS as failed.");
            }
            context.user.showAlert("SMS Failed", "Could not send SMS to " + common::to_string(to));
        }
    }


} // namespace ue