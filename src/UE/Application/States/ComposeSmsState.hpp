#pragma once

#include "BaseState.hpp"

namespace ue
{

class ComposeSmsState : public BaseState
{
public:
    ComposeSmsState(Context &context);

    void handleUiAction(std::optional<std::size_t> selectedIndex) override;
    void handleUiBack() override;

    void handleDisconnected() override;

    void handleSmsReceived(common::PhoneNumber from, std::string text) override;
    void handleSmsSentResult(common::PhoneNumber to, bool success) override;
};

} // namespace ue