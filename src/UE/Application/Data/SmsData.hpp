#pragma once

#include "../../../COMMON/Messages/PhoneNumber.hpp"
#include <string>

namespace ue
{
namespace data
{

struct SmsData
{
    common::PhoneNumber from;
    std::string text;
    bool isRead{false}; // SMS starts as unread
    // Add timestamp or other metadata if needed later
};

} // namespace data
} // namespace ue