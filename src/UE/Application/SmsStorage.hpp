#pragma once

#include "Messages/PhoneNumber.hpp"
#include <map>
#include <string>
#include <vector>
#include <optional>

namespace ue{

struct SmsMessage{
    enum class Dir { in, out };
    enum class Status { receiveR, receiveUR, sent, failed };

    common::PhoneNumber sender;
    std::string text;
    Dir direction;
    Status status;


    SmsMessage(common::PhoneNumber from, std::string text)
       : sender(from), text(text), direction(Dir::in), status(Status::receiveUR)
    {}

    SmsMessage(common::PhoneNumber to, std::string text, Status initialStatus)
        : sender(to), text(text), direction(Dir::out), status(initialStatus)
    {}

};

class SmsStorage{

    std::optional<std::size_t> lastSmsIndex;
    std::vector<SmsMessage> messages;


public:
    SmsStorage() = default;

    std::size_t addMessage(common::PhoneNumber sender, const std::string& text);
    std::size_t addSentMessage(common::PhoneNumber to, const std::string& text, SmsMessage::Status initialStatus = SmsMessage::Status::sent);
    const std::vector<SmsMessage>& getAllMessages() const;
    std::size_t getUnreadCount() const;
    bool markAsRead(std::size_t idx);
    bool markSmsOutFailed();
};

}
