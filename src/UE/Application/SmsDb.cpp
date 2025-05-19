#include "SmsDb.hpp"

namespace ue
{

void SmsDb::addSms(common::PhoneNumber from, const std::string& text)
{
    smsMessages.emplace_back(from, text, false);
}

void SmsDb::addSentSms(common::PhoneNumber from, common::PhoneNumber to, const std::string& text)
{
    smsMessages.emplace_back(from, to, text);
}

const std::vector<Sms>& SmsDb::getAllSms() const
{
    return smsMessages;
}

unsigned int SmsDb::getUnreadCount() const
{
    unsigned int count = 0;
    for (const auto& sms : smsMessages)
    {
        if (!sms.isRead)
        {
            count++;
        }
    }
    return count;
}

void SmsDb::markAsRead(size_t index)
{
    if (index < smsMessages.size())
    {
        smsMessages[index].isRead = true;
    }
}

bool SmsDb::hasUnreadSms() const
{
    for (const auto& sms : smsMessages)
    {
        if (!sms.isRead)
        {
            return true;
        }
    }
    return false;
}

}
