#pragma once

#include "BaseState.hpp"
#include <cstddef> // For std::size_t

namespace ue
{

class ViewingSingleSmsState : public BaseState
{
public:
    // Constructor takes the index of the SMS to view
    ViewingSingleSmsState(Context& context, std::size_t smsIndex);

    void handleUserAction(const std::string& id) override;

private:
    std::size_t smsIndex; // Store the index of the SMS being viewed
};

} // namespace ue