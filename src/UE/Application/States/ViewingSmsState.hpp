#pragma once

#include "BaseState.hpp"

namespace ue
{

    class ViewingSmsState: public BaseState
    {
    public:
        ViewingSmsState(Context &context, std::size_t smsIndex);

        void handleUiBack() override;
        void handleDisconnected() override;
        void handleSmsReceive(common::PhoneNumber from, std::string messageText) override;
        void handleSib(common::BtsId btsId) override;
        void handleUiAction(std::optional<std::size_t> ind) override;
        void handleCallRequest(common::PhoneNumber from) override;

    private:
        std::size_t viewingSmsIndex;
    };

}