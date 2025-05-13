#pragma once

#include "BaseState.hpp"

namespace ue
{

    class ViewingSingleSmsState : public BaseState
    {
    public:
        // Constructor now takes the index of the SMS to view
        ViewingSingleSmsState(Context &context, std::size_t smsIndex);

        void handleUiBack() override;
        void handleUiAction(std::optional<std::size_t> selectedIndex) override;

        // Still need to handle potential disconnections etc.
        void handleDisconnected() override;
        void handleSmsReceived(common::PhoneNumber from, std::string text) override;
        void handleCallRequest(common::PhoneNumber from) override;

    private:
        std::size_t viewingSmsIndex; // Index in the SmsDb vector
    };

}