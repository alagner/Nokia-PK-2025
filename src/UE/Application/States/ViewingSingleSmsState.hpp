#pragma once

#include "BaseState.hpp"
#include <cstddef>

namespace ue
{

class ViewingSingleSmsState : public BaseState
{
public:
    ViewingSingleSmsState(Context& context, std::size_t smsIndex);

    void handleUserAction(const std::string& id) override;
    // Add handleSms override declaration
    void handleSms(const common::PhoneNumber& from, const std::string& text) override;

private:
    std::size_t smsIndex;
};

} 