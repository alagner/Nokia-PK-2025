#pragma once

#include <Messages/PhoneNumber.hpp>
#include <string>

namespace ue
{

struct SmsEntity {
    common::PhoneNumber from;
    std::string text;
};
inline bool operator == (const SmsEntity& lhs, const SmsEntity& rhs)
{
    return lhs.from == rhs.from && lhs.text == rhs.text;
}
}
