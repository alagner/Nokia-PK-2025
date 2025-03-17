#pragma once

#include "BaseState.hpp"

namespace ue
{

class ConnectingState : public BaseState
{
public:
    ConnectingState(Context& context, common::BtsId btsId);

    void handleTimeout() override;
    void handleAttachAccept() override;
    void handleAttachReject() override;

private:
    common::BtsId btsId;
};

}