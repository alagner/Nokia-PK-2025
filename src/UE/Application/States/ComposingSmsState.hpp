#pragma once

#include "BaseState.hpp"

namespace ue
{

    class ComposingSmsState : public BaseState
    {
    public:
        ComposingSmsState(Context &context);

        void handleUiAction(std::optional<std::size_t> selectedIndex) override;
        void handleUiBack() override;

        void handleDisconnected() override;

        void handleSmsReceived(common::PhoneNumber from, std::string text) override;
        void handleSmsSentResult(common::PhoneNumber to, bool success) override;
        void handleCallRequest(common::PhoneNumber from) override;
    };

} // namespace ue