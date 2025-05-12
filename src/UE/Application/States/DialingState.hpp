#pragma once

#include "BaseState.hpp"

namespace ue {

class DialingState : public BaseState
{
public:
    DialingState(Context& context, common::PhoneNumber callee);
    // TODO:
    // void handleCallAccept() override;
    // void handleCallReject() override;
    void handleUiBack() override;
    void handleDisconnected() override;

private:
    common::PhoneNumber callee;
};

}
