#pragma once

#include "BaseState.hpp"

namespace ue
{

class ConnectedState : public BaseState
{
public:
    ConnectedState(Context& context);

    void handleSms(const common::PhoneNumber& from, const std::string& text) override;
    void handleUserAction(const std::string& id) override;

    void handleCallRequest(common::PhoneNumber from) override;
    void handleCallAccepted(common::PhoneNumber from) override;
    void handleCallDropped(common::PhoneNumber from) override;
    void handleCallTalk(common::PhoneNumber from, const std::string& text) override;

};

} // namespace ue