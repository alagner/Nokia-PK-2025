#pragma once

#include "BaseState.hpp"

namespace ue
{

class DialingState : public BaseState
{
public:
    DialingState(Context& context);
    void handleUserAction(const std::string& id) override;
    void handleTimeout() override;

    // Call event handlers specific to dialing
    void handleCallAccepted(common::PhoneNumber from) override;
    void handleCallDropped(common::PhoneNumber from) override;
};

} // namespace ue