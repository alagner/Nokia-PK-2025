#pragma once

#include "BaseState.hpp"

namespace ue
{

class ComposingSmsState : public BaseState
{
public:
    ComposingSmsState(Context& context);

    void handleUserAction(const std::string& id) override;
};

}