#pragma once

#include "States/BaseState.hpp"
#include "Context.hpp"

namespace ue
{

class IncomingCallState : public BaseState
{
public:
    IncomingCallState(Context& context, common::PhoneNumber from);

    void handleUiAction(std::optional<std::size_t> action) override;
    void handleUiBack() override;

private:
    common::PhoneNumber caller;
};

}
