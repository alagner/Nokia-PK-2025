#pragma once

#include "Ports/IBtsPort.hpp"
#include "Ports/IUserPort.hpp"
#include "Ports/ITimerPort.hpp"
#include "Messages/PhoneNumber.hpp"
#include <optional>

namespace ue{
    namespace view_details {
        using GuiViewMode = int;
        constexpr GuiViewMode VM_DEFAULT = 0;
        constexpr GuiViewMode VM_MENU = 1;
        constexpr GuiViewMode VM_MESSAGE_LIST = 2;
        constexpr GuiViewMode VM_MESSAGE_VIEW = 3;
        constexpr GuiViewMode VM_MESSAGE_COMPOSE = 4;
        constexpr GuiViewMode VM_MESSAGE_MENU = 5;
    }


    class IEventsHandler : public IBtsEventsHandler,
                           public ITimerEventsHandler,
                           public IUserEventsHandler
    {
    public:
        virtual ~IEventsHandler() = default;

        virtual void handleUiAction(std::optional<std::size_t> selectedIndex) = 0;
        virtual void handleUiBack() = 0;
        virtual void handleMessageReceive(common::PhoneNumber from, std::string text) = 0;
        virtual void handleMessageSentResult(common::PhoneNumber to, bool success) = 0;
        virtual void handleMessageComposeResult(common::PhoneNumber reciver, const std::string& text) = 0;

    };

}