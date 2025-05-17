#pragma once
#include "BaseState.hpp"

namespace ue
{
class DiallingState : public BaseState
{
public:
    DiallingState(Context& context);
    void handleUiAction(std::optional<std::size_t> selectedIndex) override;
    void handleUiBack() override;
};
}