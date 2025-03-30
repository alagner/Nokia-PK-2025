#pragma once

#include "Messages/PhoneNumber.hpp"
#include <string>
#include <vector>
#include <map>

namespace ue
{

/**
 * @brief Structure representing a single SMS message
 */
struct SmsMessage
{
    common::PhoneNumber from;
    std::string text;
    bool isRead;

    SmsMessage(common::PhoneNumber from, std::string text, bool isRead = false)
        : from(from), text(text), isRead(isRead)
    {}
};

/**
 * @brief Class for managing SMS storage
 */
class SmsDb
{
public:
    SmsDb() = default;

    /**
     * @brief Add a new SMS to the database
     * @param from Sender's phone number
     * @param text SMS text
     * @return Index of the stored SMS
     */
    std::size_t addSms(common::PhoneNumber from, const std::string& text);

    /**
     * @brief Get all SMS messages
     * @return Vector of SMS messages
     */
    const std::vector<SmsMessage>& getAllSms() const;

    /**
     * @brief Get count of unread SMS messages
     * @return Number of unread SMS messages
     */
    std::size_t getUnreadCount() const;

    /**
     * @brief Mark SMS as read
     * @param index Index of the SMS to mark as read
     * @return true if successful, false if index is invalid
     */
    bool markAsRead(std::size_t index);

private:
    std::vector<SmsMessage> messages;
};

} // namespace ue
