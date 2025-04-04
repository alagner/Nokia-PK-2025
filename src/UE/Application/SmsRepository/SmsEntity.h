#pragma once

#include <Messages/PhoneNumber.hpp>
#include <string>

namespace ue
{

struct SmsEntity {
    common::PhoneNumber from;
    common::PhoneNumber to;
    std::string text;
};
inline bool operator == (const SmsEntity& lhs, const SmsEntity& rhs)
{
    return lhs.from == rhs.from && lhs.to == rhs.to && lhs.text == rhs.text;
}
}
