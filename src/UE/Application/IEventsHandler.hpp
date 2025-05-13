#pragma once

#include "Ports/IBtsPort.hpp"
#include "Ports/IUserPort.hpp"
#include "Ports/ITimerPort.hpp"
#include "Messages/PhoneNumber.hpp"
#include <optional>

namespace ue{
    namespace view_details {

        enum class GuiViewMode {
            Default,            //0
            Menu,               //1
            Message_list,       //2
            Message_view,       //3
            Message_compose,    //4
            Message_menu,       //5
            Call_menu           //6
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