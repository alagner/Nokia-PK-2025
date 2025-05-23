#pragma once

#include "BaseState.hpp"

namespace ue
{

    class ViewingSmsListState : public BaseState
    {
    public:
        explicit ViewingSmsListState(Context& context);

        void handleUiAction(std::optional<std::size_t> selectedIndex) override;
        void handleUiBack() override;
        void handleDisconnected() override;
        void handleSmsReceive(common::PhoneNumber from, std::string messageText) override;
        void handleSib(common::BtsId btsId) override;
        void handleCallRequest(common::PhoneNumber from) override;

    private:
        void showList();
        std::vector<SmsMessage> currentSmsList;
    };

}