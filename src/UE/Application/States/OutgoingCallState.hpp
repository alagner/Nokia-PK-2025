#pragma once

#include "BaseState.hpp"
#include "Messages/PhoneNumber.hpp"

namespace ue
{

    class OutgoingCallState : public BaseState
    {
    public:
        OutgoingCallState(Context& context, common::PhoneNumber to);
        ~OutgoingCallState() override = default;

        void handleUiBack() override;
        void handleCallDropped(common::PhoneNumber to) override;
        void handleDisconnected() override;
        void handleUiAction(std::optional<std::size_t> ind) override;
        void handleTalkCall(common::PhoneNumber from, const std::string &message) override;
        void handleTimeout() override;
        void handleNumberUnknown(common::PhoneNumber to) override;
        void handleSmsReceive(common::PhoneNumber from, std::string messageText) override;
        void handleCallRequest(common::PhoneNumber from) override;

    private:
        common::PhoneNumber callee;
        bool awaitingAfterEnd = false;
    };

}