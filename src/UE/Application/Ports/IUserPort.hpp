#pragma once
#include "Messages/PhoneNumber.hpp"
#include <vector>
#include "SmsRepository/SmsEntity.h"


namespace ue
{

class IUserEventsHandler
{
public:
    virtual void viewSmsList() = 0;
    virtual void viewSms(unsigned) = 0;
    virtual ~IUserEventsHandler() = default;
};

class IUserPort
{
public:
    virtual ~IUserPort() = default;

    virtual void showNotConnected() = 0;
    virtual void showConnecting() = 0;
    virtual void showConnected() = 0;
    virtual void showSmsList(const std::vector<SmsEntity> &) = 0;
    virtual void showSms(const SmsEntity &) = 0;
    virtual void showNewSms() = 0;
};

}
