#pragma once

#include "Messages/PhoneNumber.hpp"
#include <vector>
#include <string>

namespace ue
{

class Sms;

class ISmsDb
{
public:
    virtual ~ISmsDb() = default;
    virtual void addSms(common::PhoneNumber from, const std::string& text) = 0;
    virtual void addSentSms(common::PhoneNumber from, common::PhoneNumber to, const std::string& text) = 0;
    virtual const std::vector<Sms>& getAllSms() const = 0;
    virtual unsigned int getUnreadCount() const = 0;
    virtual void markAsRead(size_t index) = 0;
    virtual bool hasUnreadSms() const = 0;
    virtual void saveToFile() const = 0;
    virtual void loadFromFile() = 0;
    virtual void clearMessages() = 0;
};

}