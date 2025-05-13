#pragma once

#include "BaseState.hpp"
#include "Messages/PhoneNumber.hpp"
#include <chrono>

namespace ue
{

    class IncomingCallState : public BaseState
    {
    public:
        IncomingCallState(Context &context, common::PhoneNumber callingNumber);
        ~IncomingCallState() override;

        void handleUiAction(std::optional<std::size_t> selectedIndex) override;
        void handleUiBack() override;
        void handleTimeout() override;
        void handleDisconnected() override;
        void handleCallDropped(common::PhoneNumber peer) override;
        void handleCallReject(common::PhoneNumber peer) override;
        void handleSmsReceived(common::PhoneNumber from, std::string text) override;
        void handleCallRequest(common::PhoneNumber from) override;

    private:
        common::PhoneNumber callingPhoneNumber;
        static constexpr std::chrono::milliseconds CALL_TIMEOUT{30000};
    };

}