#include "SmsDb.hpp"

namespace ue
{

    std::size_t SmsDb::addReceivedSms(common::PhoneNumber from, const std::string &text)
    {
        messages.emplace_back(from, text); // Uses incoming constructor
        return messages.size() - 1;
    }

    std::size_t SmsDb::addSentSms(common::PhoneNumber to, const std::string &text, SmsMessage::Status initialStatus)
    {
        messages.emplace_back(to, text, initialStatus); // Uses outgoing constructor
        lastSentSmsIndex = messages.size() - 1;         // Track last sent index
        return messages.size() - 1;
    }

    const std::vector<SmsMessage> &SmsDb::getAllSms() const
    {
        return messages;
    }

    std::size_t SmsDb::getUnreadCount() const
    {
        std::size_t count = 0;
        for (const auto &message : messages)
        {
            // Only count incoming messages that are unread
            if (message.direction == SmsMessage::Direction::INCOMING && message.status == SmsMessage::Status::RECEIVED_UNREAD)
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

        // Only mark incoming, unread messages as read
        if (messages[index].direction == SmsMessage::Direction::INCOMING && messages[index].status == SmsMessage::Status::RECEIVED_UNREAD)
        {
            messages[index].status = SmsMessage::Status::RECEIVED_READ;
            return true;
        }
        return false; // Not an incoming unread message
    }

    bool SmsDb::markLastOutgoingSmsAsFailed()
    {
        if (lastSentSmsIndex.has_value() && lastSentSmsIndex.value() < messages.size())
        {
            auto &msg = messages[lastSentSmsIndex.value()];
            // Make sure it's still the same outgoing message we intended
            if (msg.direction == SmsMessage::Direction::OUTGOING)
            {
                msg.status = SmsMessage::Status::FAILED;
                lastSentSmsIndex.reset(); // Reset so we don't mark it again accidentally
                return true;
            }
        }
        // Fallback: Search backwards for the most recent outgoing message if index tracking fails
        for (int i = messages.size() - 1; i >= 0; --i)
        {
            if (messages[i].direction == SmsMessage::Direction::OUTGOING && messages[i].status == SmsMessage::Status::SENT)
            {
                messages[i].status = SmsMessage::Status::FAILED;
                return true;
            }
        }
        return false; // No suitable outgoing message found
    }

} // namespace ue
