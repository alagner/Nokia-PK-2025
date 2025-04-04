//
// Created by student on 4/4/25.
//
#pragma once

#ifndef SMSSTORAGE_HPP
#define SMSSTORAGE_HPP

#endif


#include "Messages/PhoneNumber.hpp"
#include <string>
#include <vector>
#include <map>


namespace ue
{

struct SmsMessage{

    common::PhoneNumber sender;
    std::string text;
    bool isRead;

    SmsMessage(common::PhoneNumber sender, std::string text, bool isRead = false) : sender(sender), text(text), isRead(isRead){}
};

class SmsStorage{

private:
    std::vector<SmsMessage> messages;

public:
    SmsStorage() = default;

    std::size_t addMessage(common::PhoneNumber sender, const std::string& text);

    const std::vector<SmsMessage>& getAllMessages() const;

    std::size_t getUnreadCount() const;

    bool markAsRead(std::size_t idx);



}



}
