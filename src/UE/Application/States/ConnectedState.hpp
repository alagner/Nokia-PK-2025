#pragma once

#include "BaseState.hpp"

namespace ue
{

class ConnectedState : public BaseState
{
public:
    ConnectedState(Context &context);

    // Override handleDisconnected to handle disconnection in connected state
    void handleDisconnected() override;
    
    // Override handleSmsReceived to handle SMS in connected state
    void handleSmsReceived(common::PhoneNumber from, std::string text) override;
};

}
