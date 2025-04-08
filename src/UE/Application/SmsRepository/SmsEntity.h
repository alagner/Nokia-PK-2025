#pragma once

#include <Messages/PhoneNumber.hpp>
#include <string>

namespace ue
{

struct SmsEntity {
    std::uint8_t from;
    std::uint8_t to;
    std::string text;
    bool isRead = 0;

};
inline bool operator == (const SmsEntity& lhs, const SmsEntity& rhs)
{
    return lhs.from == rhs.from && lhs.text == rhs.text && lhs.to == rhs.to && lhs.isRead == rhs.isRead;
}
}
