#pragma once

#include "BaseState.hpp"

namespace ue
{

// Renamed from ConnectedState
class ConnectedState : public BaseState
{
public:
    ConnectedState(Context &context);

    // Override handlers relevant to the connected state
    void handleDisconnected() override;
    void handleSmsReceived(common::PhoneNumber from, std::string text) override;
    void handleSmsSentResult(common::PhoneNumber to, bool success) override;

    // Handle UI Actions from the main menu
    void handleUiAction(std::optional<std::size_t> selectedIndex) override;
    void handleUiBack() override; // Back from main menu might do nothing or exit app?

private:
    void showMainMenu(); // Helper to display the menu via UserPort
};

}