#pragma once

#include "BaseState.hpp"

namespace ue
{

class ViewingSmsListState : public BaseState
{
public:
    ViewingSmsListState(Context& context);

    // Handle UI Actions from the SMS list
    void handleUiAction(std::optional<std::size_t> selectedIndex) override;
    void handleUiBack() override;

    // Still need to handle potential disconnections etc.
    void handleDisconnected() override;
    void handleSmsReceived(common::PhoneNumber from, std::string text) override;


private:
    void showList(); // Helper to fetch and display list
    std::vector<SmsMessage> currentSmsList; // Store the list being displayed
};

}