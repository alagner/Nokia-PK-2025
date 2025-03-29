#pragma once

#include "Logger/PrefixedLogger.hpp"
#include "Messages/PhoneNumber.hpp"
#include "IEventsHandler.hpp"
#include "Context.hpp"
#include "Ports/IBtsPort.hpp"  
#include "Ports/IUserPort.hpp" 
#include "Ports/ITimerPort.hpp"
#include "SmsDb.hpp"

namespace ue
{

    using common::ILogger;
    using common::PhoneNumber;

    class Application : public IEventsHandler
    {
    public:
        Application(PhoneNumber phoneNumber,
                    ILogger &iLogger,
                    IBtsPort &bts,
                    IUserPort &user,
                    ITimerPort &timer);
        ~Application();
        
        void handleUiAction(std::optional<std::size_t> selectedIndex) override;
        void handleUiBack() override;
        // ITimerEventsHandler interface
        void handleTimeout() override;

        // IBtsEventsHandler interface
        void handleSib(common::BtsId btsId) override;
        void handleAttachAccept() override;
        void handleAttachReject() override;
        void handleDisconnected() override;
        void handleSmsReceived(common::PhoneNumber from, std::string text) override;

    private:
        Context context;
        common::PrefixedLogger logger;
    };

}
