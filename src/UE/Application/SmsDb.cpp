#include "SmsDb.hpp"

namespace ue
{

std::size_t SmsDb::addSms(common::PhoneNumber from, const std::string& text)
{
    messages.emplace_back(from, text, false);
    return messages.size() - 1;
}

const std::vector<SmsMessage>& SmsDb::getAllSms() const
{
    return messages;
}

std::size_t SmsDb::getUnreadCount() const
{
    std::size_t count = 0;
    for (const auto& message : messages)
    {
        if (!message.isRead)
        {
            count++;
        }
    }
    return count;
}

bool SmsDb::markAsRead(std::size_t index)
{
    if (index >= messages.size())
    {
        return false;
    }
    
    messages[index].isRead = true;
    return true;
}

} // namespace ue
