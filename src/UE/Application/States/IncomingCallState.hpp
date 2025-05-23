#pragma once

#include "BaseState.hpp"
#include "../Context.hpp"
#include <chrono>

namespace ue
{

    class IncomingCallState : public BaseState
    {
    public:
        IncomingCallState(Context& context, common::PhoneNumber from);
        ~IncomingCallState() override;

        void handleUiAction(std::optional<std::size_t> action) override;
        void handleUiBack() override;
        void handleTimeout() override;
        void handleDisconnected() override;
        void handleCallDropped(common::PhoneNumber to) override;
        void handleCallReject(common::PhoneNumber to) override;
        void handleSmsReceive(common::PhoneNumber from, std::string messageText) override;
        void handleCallRequest(common::PhoneNumber from) override;

    private:
        common::PhoneNumber caller;
        static constexpr std::chrono::milliseconds TIMEOUT{30000};
    };

}