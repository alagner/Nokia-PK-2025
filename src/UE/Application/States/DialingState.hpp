#pragma once

#include "BaseState.hpp"

namespace ue
{

    class DialingState : public BaseState
    {
    public:
        DialingState(Context &context);
        ~DialingState() override;

        void handleUiAction(std::optional<std::size_t> selectedIndex) override;
        void handleUiBack() override;
        void handleAcceptCall(common::PhoneNumber to) override;
        void handleDisconnected() override;
        void handleTimeout() override;
        void handleNumberUnknown(common::PhoneNumber to) override;
        void handleCallReject(common::PhoneNumber to) override;
        void handleSmsReceive(common::PhoneNumber from, std::string messageText) override;
        void handleCallRequest(common::PhoneNumber from) override;
        void handleCallDropped(common::PhoneNumber from) override;


    private:
        common::PhoneNumber diallingNumber;
        bool awaitingAfterFail = false;
    };

}