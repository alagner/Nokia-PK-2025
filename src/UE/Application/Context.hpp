#pragma once

#include "IEventsHandler.hpp"
#include "Ports/IBtsPort.hpp"
#include "Ports/IUserPort.hpp"
#include "Ports/ITimerPort.hpp"
#include "Logger/ILogger.hpp"
#include "SmsDb.hpp"
#include "Messages/PhoneNumber.hpp"
#include <memory>

namespace ue
{

    struct Context
    {
        common::ILogger &logger;
        IBtsPort &bts;
        IUserPort &user;
        ITimerPort &timer;
        SmsDb smsDb;
        std::unique_ptr<IEventsHandler> state{};
        common::PhoneNumber myPhoneNumber;

        template <typename State, typename... Arg>
        void setState(Arg &&...arg)
        {
            state = std::make_unique<State>(*this, std::forward<Arg>(arg)...);
        }
    };

}
