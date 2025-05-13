#pragma once

#include "BaseState.hpp"

namespace ue
{

    class CallingState : public BaseState
    {
    public:
        CallingState(Context &context);

        void handleUiAction(std::optional<std::size_t> selectedIndex) override;
        void handleUiBack() override;
        void handleCallAccept(common::PhoneNumber peer) override;
        void handleDisconnected() override;
        void handleTimeout() override;
        void handleUnknownRecipient(common::PhoneNumber peer) override;
        void handleCallReject(common::PhoneNumber peer) override;
        void handleSmsReceived(common::PhoneNumber from, std::string text) override;
        void handleCallRequest(common::PhoneNumber from) override;

    private:
        common::PhoneNumber dialedNumber;
        bool awaitingUserAfterFailure = false;
    };

}
