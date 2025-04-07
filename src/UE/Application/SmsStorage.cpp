//
// Created by student on 4/4/25.
//
#include "SmsStorage.hpp"

namespace ue
{

std::size_t SmsStorage::addMessage(common::PhoneNumber sender, const std::string& text)
{
    messages.emplace_back(sender, text, false);
    return messages.size() - 1;
}

const std::vector<SmsMessage>& SmsStorage::getAllMessages() const
{
    return messages;
}

std::size_t SmsStorage::getUnreadCount() const
{
    std::size_t counter = 0;

    for (auto sms: messages)
    {
        if (!sms.isRead)
            counter++;
    }

    return counter;
}

bool SmsStorage::markAsRead(std::size_t idx)
{
    if (idx >= messages.size())
        return false;

    messages[idx].isRead = true;
    return true;
}

}