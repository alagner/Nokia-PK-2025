#pragma once

#include "BaseState.hpp"
#include <optional>
#include "Context.hpp"
#include "SmsStorage.hpp"

namespace ue{

    class ComposeSmsState : public BaseState{
    public:
        ComposeSmsState(Context &context);

        void handleUiAction(std::optional<std::size_t> selectedIndex) override;
        void handleUiBack() override;

        void handleDisconnected() override;

        void handleMessageReceive(common::PhoneNumber from, std::string text) override;
        void handleMessageSentResult(common::PhoneNumber to, bool success) override;
    };

}