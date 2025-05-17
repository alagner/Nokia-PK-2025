#pragma once

#include "BaseState.hpp"
#include "SmsStorage.hpp"
#include "IUeGui.hpp"
#include <vector>
#include <optional>

namespace ue {

    class ViewListSmsState : public BaseState {
    public:
        explicit ViewListSmsState(Context& context);

        void handleUiAction(std::optional<std::size_t> selectedIndex) override;
        void handleUiBack() override;
        void handleDisconnected() override;
        void handleMessageReceive(common::PhoneNumber from, std::string text) override;

    private:
        void refreshList();
        void logSmsListDebug();
        bool isRead(const SmsMessage& msg) const;

        std::vector<SmsMessage> messages_;
    };

}
