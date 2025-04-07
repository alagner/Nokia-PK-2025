#pragma once

#include "Messages/PhoneNumber.hpp"
#include <string>
#include <optional> // Include optional for 'to' field

namespace ue
{
namespace data
{

struct SmsData
{
    // For received SMS: 'from' is sender, 'to' is empty.
    // For sent SMS: 'from' is self (optional), 'to' is recipient.
    common::PhoneNumber from{}; // Sender (if received)
    std::optional<common::PhoneNumber> to{}; // Recipient (if sent)
    std::string text{};
    bool isRead{false}; // Relevant mainly for received SMS
    bool isSent{false}; // Flag to indicate sent vs received

    // Add timestamp or other metadata if needed later
    // bool sendFailed{false}; // Could add later for UnknownRecipient handling
};

} // namespace data
} // namespace ue