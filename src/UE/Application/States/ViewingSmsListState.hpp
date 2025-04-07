#pragma once

#include "BaseState.hpp"

namespace ue
{

class ViewingSmsListState : public BaseState
{
public:
    ViewingSmsListState(Context& context);

    void handleUserAction(const std::string& id) override;
};

} // namespace ue