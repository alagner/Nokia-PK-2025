#pragma once

#include "BaseState.hpp"
#include "Messages/PhoneNumber.hpp"

namespace ue
{

    class OutgoingCallState : public BaseState
    {
    public:
        OutgoingCallState(Context& context, common::PhoneNumber to);

        void handleCallRequest(common::PhoneNumber from) override;
        void handleUiBack() override;

    private:
        common::PhoneNumber callee;
    };

}