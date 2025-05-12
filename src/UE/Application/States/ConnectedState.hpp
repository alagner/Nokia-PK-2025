#pragma once

#include "BaseState.hpp"

namespace ue
{

class ConnectedState : public BaseState
{
public:
    explicit ConnectedState(Context &context);
    void handleDisconnected() override;
    void handleSmsReceive(common::PhoneNumber from, std::string messageText) override;
    void handleUiAction(std::optional<std::size_t> selectedIndex) override;
    void handleUiBack() override;
    void handleSmsSent(common::PhoneNumber to, bool succeed) override;

    void handleSib(common::BtsId) override;
private:
    void showMenu();

    //TODO consider

};

}
