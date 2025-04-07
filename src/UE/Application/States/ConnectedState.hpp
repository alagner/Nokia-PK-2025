#pragma once

#include "BaseState.hpp"

namespace ue
{

class ConnectedState : public BaseState
{
public:
    ConnectedState(Context& context);

    void handleSms(const common::PhoneNumber& from, const std::string& text) override;
    // CORRECTED: Added handleUserAction override declaration
    void handleUserAction(const std::string& id) override;
};

} // namespace ue