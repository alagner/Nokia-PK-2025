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

        void handleSmsReceive(common::PhoneNumber from, std::string textMessage) override;
        void handleSmsSent(common::PhoneNumber to, bool succeed) override;
        void handleCallRequest(common::PhoneNumber from) override;
    };

} // namespace ue