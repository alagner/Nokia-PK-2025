#pragma once

#include "IEventsHandler.hpp"
#include "Logger/ILogger.hpp"
#include "ISmsDb.hpp"
#include <memory>

namespace ue
{

struct Context
{
    common::ILogger& logger;
    IBtsPort& bts;
    IUserPort& user;
    ITimerPort& timer;
    common::PhoneNumber phoneNumber;
    ISmsDb& smsDb;
    std::unique_ptr<IEventsHandler> state{};
    common::BtsId currentBtsId{};  

    Context(common::ILogger& logger, IBtsPort& bts, IUserPort& user, 
            ITimerPort& timer, common::PhoneNumber number, ISmsDb& smsDb)
        : logger(logger), bts(bts), user(user), timer(timer), 
          phoneNumber(number), smsDb(smsDb) {}

    template <typename State, typename ...Arg>
    void setState(Arg&& ...arg)
    {
        state = std::make_unique<State>(*this, std::forward<Arg>(arg)...);
    }
};

}
