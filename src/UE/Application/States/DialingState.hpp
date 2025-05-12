#pragma once

#include "BaseState.hpp"

namespace ue
{

    class DialingState : public BaseState
    {
    public:
        DialingState(Context &context);

        void handleUiAction(std::optional<std::size_t> selectedIndex) override;
        void handleUiBack() override;
//        void handleCallAccept(common::PhoneNumber peer) override;
        void handleDisconnected() override;
        void handleTimeout() override;
//        void handleUnknownRecipient(common::PhoneNumber peer) override;
//        void handleCallReject(common::PhoneNumber peer) override;

    private:
        common::PhoneNumber diallingNumber;
    };

}