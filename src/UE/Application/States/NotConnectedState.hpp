#pragma once

#include "BaseState.hpp"

namespace ue
{

class NotConnectedState : public BaseState
{
public:
    NotConnectedState(Context& context);

public:
    void handleSib(common::BtsId) override;
};

}
