#pragma once

#include "Ports/IBtsPort.hpp"
#include "Ports/IUserPort.hpp"
#include "Ports/ITimerPort.hpp"
#include "Messages/PhoneNumber.hpp"

namespace ue
{
    namespace details {
        enum class GuiViewMode {
            Unknown,
            MainMenu,
            SmsList,
            SmsView,
            SmsCompose,
            SmsMenu
            //TODO add more here
        };
    }

class IEventsHandler : public IBtsEventsHandler,
                       public ITimerEventsHandler,
                       public IUserEventsHandler
{
public:
    virtual ~IEventsHandler() = default;
    virtual void handleUiAction(std::optional<std::size_t> ind) = 0;
    virtual void handleUiBack() = 0;
    //virtual void handleSmsReceive(common::PhoneNumber from, std::string textMessage) = 0;
    virtual void handleSmsSent(common::PhoneNumber to, bool succeed) = 0;
    virtual void handleSmsCompose(common::PhoneNumber to, const std::string& textMessage) = 0;
};

}
