#pragma once

#include "Ports/IBtsPort.hpp"
#include "Ports/IUserPort.hpp"
#include "Ports/ITimerPort.hpp"
#include "Messages/PhoneNumber.hpp"
#include <optional>

namespace ue{
    namespace view_details {
        using GuiViewMode = int;

        enum class GuiViewMode {
            default,            //0
            menu,               //1
            message_list,       //2
            message_view,       //3
            message_compose,    //4
            message_menu,       //5
            call_menu           //6
        };
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