#pragma once

#include "BaseState.hpp"

namespace ue
{

    class ViewingSingleSmsState : public BaseState
    {
    public:
        // Constructor now takes the index of the SMS to view
        ViewingSingleSmsState(Context &context, std::size_t smsIndex);

        // Only Back action is relevant here (Accept does nothing for now)
        void handleUiBack() override;

        // Still need to handle potential disconnections etc.
        void handleDisconnected() override;
        void handleSmsReceived(common::PhoneNumber from, std::string text) override;

    private:
        std::size_t viewingSmsIndex; // Index in the SmsDb vector
    };

}