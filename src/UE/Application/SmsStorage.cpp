#include "SmsStorage.hpp"

namespace ue{

std::size_t SmsStorage::addMessage(common::PhoneNumber sender, const std::string& text){
    messages.emplace_back(sender, text);
    return messages.size() - 1;
}

std::size_t SmsStorage::addSentMessage(common::PhoneNumber to, const std::string &text, SmsMessage::Status initialStatus){
    messages.emplace_back(to, text, initialStatus);
    lastSmsIndex = messages.size() - 1;
    return messages.size() - 1;
}

const std::vector<SmsMessage>& SmsStorage::getAllMessages() const{
    return messages;
}

std::size_t SmsStorage::getUnreadCount() const{
    std::size_t counter = 0;

    for (auto sms: messages){
        if (sms.direction == SmsMessage::Dir::in && sms.status == SmsMessage::Status::receiveUR)
            counter++;
    }

    return counter;
}

bool SmsStorage::markAsRead(std::size_t index){
    if (index >= messages.size()){
        return false;
    }

    if (messages[index].direction == SmsMessage::Dir::in && messages[index].status == SmsMessage::Status::receiveUR){
        messages[index].status = SmsMessage::Status::receiveR;
        return true;
    }
    return false;
}

bool SmsStorage::markSmsOutFailed(){
    if (lastSmsIndex.has_value() && lastSmsIndex.value() < messages.size()){
        auto &msg = messages[lastSmsIndex.value()];
        if (msg.direction == SmsMessage::Dir::out)
        {
            msg.status = SmsMessage::Status::failed;
            lastSmsIndex.reset();
            return true;
        }
    }
    for (int i = messages.size() - 1; i >= 0; --i){
        if (messages[i].direction == SmsMessage::Dir::out && messages[i].status == SmsMessage::Status::sent){
            messages[i].status = SmsMessage::Status::failed;
            return true;
        }
    }
    return false;
}
}