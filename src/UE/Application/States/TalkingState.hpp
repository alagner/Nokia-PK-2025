#pragma once

#include "BaseState.hpp"
#include "Messages/PhoneNumber.hpp"

namespace ue
{

    class TalkingState : public BaseState
    {
    public:
        TalkingState(Context &context, common::PhoneNumber peerNumber);
        ~TalkingState() override = default;

        void handleUiBack() override;
        void handleCallDropped(common::PhoneNumber peer) override;
        void handleDisconnected() override;
        void handleUiAction(std::optional<std::size_t>) override;
        void handleCallTalk(common::PhoneNumber from, const std::string &text) override;
        void handleTimeout() override;
        void handleUnknownRecipient(common::PhoneNumber peer) override;
        void handleSmsReceived(common::PhoneNumber from, std::string text) override;

    private:
        common::PhoneNumber peerPhoneNumber;
        bool awaitingUserAfterCallEnd = false;
    };

}