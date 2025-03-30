#pragma once

#include "Messages/PhoneNumber.hpp"
#include <string>
#include <vector>
#include <map>
#include <optional>

namespace ue
{

/**
 * @brief Structure representing a single SMS message
 */
struct SmsMessage
{
    enum class Direction { INCOMING, OUTGOING };
    enum class Status { RECEIVED_READ, RECEIVED_UNREAD, SENT, FAILED };

    common::PhoneNumber peer;
    std::string text;
    //bool isRead;
    Direction direction;
    Status status;

    // Constructor for incoming SMS
    SmsMessage(common::PhoneNumber from, std::string text)
        : peer(from), text(text), direction(Direction::INCOMING), status(Status::RECEIVED_UNREAD)
    {}

    // Constructor for outgoing SMS
    SmsMessage(common::PhoneNumber to, std::string text, Status initialStatus)
        : peer(to), text(text), direction(Direction::OUTGOING), status(initialStatus)
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
    std::size_t addReceivedSms(common::PhoneNumber from, const std::string& text);

    /**
     * @brief Add a new *outgoing* SMS to the database
     * @param to Recipient's phone number
     * @param text SMS text
     * @param initialStatus The initial status (e.g., SENT)
     * @return Index of the stored SMS
     */
    std::size_t addSentSms(common::PhoneNumber to, const std::string& text, SmsMessage::Status initialStatus = SmsMessage::Status::SENT);

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

    /**
     * @brief Marks the last sent (outgoing) SMS as failed.
     * @return true if an outgoing SMS was found and marked, false otherwise.
     */
    bool markLastOutgoingSmsAsFailed();

private:
    std::vector<SmsMessage> messages;
    std::optional<std::size_t> lastSentSmsIndex;
};

} // namespace ue
