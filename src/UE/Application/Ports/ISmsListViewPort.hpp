#pragma once

#include "Messages/PhoneNumber.hpp"
#include "../SmsDb.hpp"
#include <vector>
#include <functional>

namespace ue
{

class ISmsListViewPort
{
public:
    virtual ~ISmsListViewPort() = default;
    virtual void setSmsList(const std::vector<Sms>& smsList) = 0;
    virtual void setSelectSmsCallback(std::function<void(size_t)> callback) = 0;
};

}
