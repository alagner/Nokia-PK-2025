#pragma once

#include "BaseState.hpp"
#include <optional>

namespace ue {

class ViewSmsState : public BaseState {
public:
    ViewSmsState(Context& context, std::size_t smsIndex);

    void handleUiBack() override;
    void handleUiAction(std::optional<std::size_t> selectedIndex) override;
    void handleDisconnected() override;
    void handleMessageReceive(common::PhoneNumber from, std::string text) override;

private:
    std::size_t smsIndex_;
    
    void displayCurrentSms();
    void returnToSmsList();
};

}
