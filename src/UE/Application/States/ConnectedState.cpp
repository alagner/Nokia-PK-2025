#include "ConnectedState.hpp"
#include "NotConnectedState.hpp"
#include "ViewingSmsListState.hpp"
#include "ComposingSmsState.hpp"
#include "IncomingCallState.hpp"
#include "CallingState.hpp"
#include "UeGui/IListViewMode.hpp"
#include "UeGui/ITextMode.hpp"
#include <vector>

namespace ue
{
    ConnectedState::ConnectedState(Context &context)
        : BaseState(context, "ConnectedState")
    {
        showMainMenu();
    }

    void ConnectedState::showMainMenu()
    {
        logger.logInfo("Entering Main Menu");
        context.user.showConnected();
    }

    void ConnectedState::handleDisconnected()
    {
        logger.logInfo("Connection to BTS lost");
        context.setState<NotConnectedState>();
    }

    void ConnectedState::handleSmsReceived(common::PhoneNumber from, std::string text)
    {
        logger.logInfo("SMS received from: ", from);
        std::size_t smsIndex = context.smsDb.addReceivedSms(from, text);
        logger.logDebug("SMS stored at index: ", smsIndex);
        context.user.showNewSms();
    }

    void ConnectedState::handleUiAction(std::optional<std::size_t> selectedIndex)
    {

        if (!selectedIndex.has_value())
        {
            logger.logInfo("UI Action received with no index in Main Menu");
            return;
        }

        logger.logInfo("Main menu selection: index=", selectedIndex.value());

        switch (selectedIndex.value())
        {
        case 0:
        {
            logger.logInfo("Compose SMS selected");
            context.setState<ComposingSmsState>();
            break;
        }

        case 1:
        {
            logger.logInfo("View SMS selected");
            context.setState<ViewingSmsListState>();
            break;
        }
        case 2:
        {
            logger.logInfo("Call selected");
            context.user.showDialMode();
            context.setState<CallingState>();
            break;
        }

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
        if (!success)
        {
            if (!context.smsDb.markLastOutgoingSmsAsFailed())
            {
                logger.logInfo("Could not mark last outgoing SMS as failed.");
            }
            context.user.showAlert("SMS Failed", "Could not send SMS to " + common::to_string(to));
        }
    }

    void ConnectedState::handleCallRequest(common::PhoneNumber from)
    {
        logger.logInfo("Handling incoming call request from: ", from);
        context.setState<IncomingCallState>(from);
    }

} // namespace ue