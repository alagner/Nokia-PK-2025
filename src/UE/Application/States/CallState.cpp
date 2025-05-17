#include "CallState.hpp"
#include "ConnectedState.hpp"
#include "Utils/todo.h"

namespace ue
{
DiallingState::DiallingState(Context& context) : BaseState(context, "DiallingState")
{
    this->logger.logInfo("Entered DiallingState");
    this->context.user.showCallMenu();
};

void DiallingState::handleUiAction(std::optional<std::size_t> selectedIndex)
{
    constexpr std::size_t dial_number_code{ 0 };
    constexpr std::size_t call_history_code{ 1 };

    if (!selectedIndex.has_value())
    {
        this->logger.logInfo("UI Action received with no index in Call menu");
        return;
    }

    switch (selectedIndex.value())
    {
    case dial_number_code: {
        this->logger.logDebug("Dial number selected");
        auto recipient{ this->context.user.getCallRecipient() };

        if (!recipient.isValid())
        {
            this->logger.logInfo("Cannot make call: Invalid receiver");
            this->context.user.showNotify("Error", "Invalid receiver");
            return;
        }

        break;
    }
    case call_history_code:
        this->logger.logDebug("Call history selected");
        TODO(Implement call history display)
        break;
    default:
        this->logger.logError("Invalid menu option selected: ", selectedIndex.value());
        break;
    }
}

void DiallingState::handleUiBack()
{
    this->context.logger.logInfo("Dialling cancelled by the user");
    this->context.setState<ConnectedState>();
}
}