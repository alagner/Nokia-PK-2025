#pragma once

#include "BaseState.hpp"

namespace ue
{

class ConnectedState : public BaseState
{
public:
    ConnectedState(Context& context);

    // handleDisconnect() is now handled globally, removing override here
    // void handleDisconnect() override;

    void handleSms(const common::PhoneNumber& from, const std::string& text) override;
    // CORRECTED: Added handleUserAction override declaration
    void handleUserAction(const std::string& id) override;
};

} // namespace ue