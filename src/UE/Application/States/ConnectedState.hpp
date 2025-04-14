#pragma once

#include "BaseState.hpp"

namespace ue{

class ConnectedState : public BaseState{
public:
    ConnectedState(Context& context);
    void handleDisconnected() override;

    void handleMessageReceive(common::PhoneNumber sender, std::string text) override;
    void handleMessageSentResult(common::PhoneNumber to, bool success) override;

    void handleUiAction(std::optional<std::size_t> selectedIndex) override;
    void handleUiBack() override;

};

}
