#pragma once

#include "BaseState.hpp"

namespace ue
{

class ViewingSmsListState : public BaseState
{
public:
    ViewingSmsListState(Context& context);

    void handleUserAction(const std::string& id) override;
    // CORRECTED: Add handleSms override declaration
    void handleSms(const common::PhoneNumber& from, const std::string& text) override;
};

}