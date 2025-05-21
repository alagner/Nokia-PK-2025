#include "SmsDb.hpp"
#include <iostream>
#include <stdexcept>

namespace ue
{

SmsDb::SmsDb()
{

}

SmsDb::SmsDb(const common::PhoneNumber& phoneNumber) : ownNumber(phoneNumber)
{
    try {
        loadFromFile();
    } catch (const std::exception& e) {

    }
}

SmsDb::~SmsDb()
{
    try {
        saveToFile();
    } catch (const std::exception& e) {

        std::cerr << "Failed to save SMS database: " << e.what() << std::endl;
    }
}

void SmsDb::addSms(common::PhoneNumber from, const std::string& text)
{
    smsMessages.emplace_back(from, text, false);
    saveToFile();
}

void SmsDb::addSentSms(common::PhoneNumber from, common::PhoneNumber to, const std::string& text)
{
    smsMessages.emplace_back(from, to, text);
    saveToFile();
}

const std::vector<Sms>& SmsDb::getAllSms() const
{
    return smsMessages;
}

unsigned int SmsDb::getUnreadCount() const
{
    unsigned int count = 0;
    for (const auto& sms : smsMessages)
    {
        if (!sms.isRead)
        {
            count++;
        }
    }
    return count;
}

void SmsDb::markAsRead(size_t index)
{
    if (index < smsMessages.size())
    {
        smsMessages[index].isRead = true;
        saveToFile();
    }
}

bool SmsDb::hasUnreadSms() const
{
    for (const auto& sms : smsMessages)
    {
        if (!sms.isRead)
        {
            return true;
        }
    }
    return false;
}

std::string SmsDb::getStorageFilePath() const
{
    const char* homeDir = std::getenv("HOME");
    std::string basePath = homeDir ? homeDir : ".";

    if (ownNumber.isValid()) {
        return basePath + "/.ue_" + std::to_string(ownNumber.value) + "_" + SMS_DB_FILENAME;
    } else {
        return basePath + "/.ue_" + SMS_DB_FILENAME;
    }
}

void SmsDb::saveToFile() const
{
    if (!ownNumber.isValid()) {
        return;
    }
    
    std::ofstream file(getStorageFilePath(), std::ios::binary);
    if (!file) {
        throw std::runtime_error("Failed to open SMS database file for writing");
    }

    size_t count = smsMessages.size();
    file.write(reinterpret_cast<const char*>(&count), sizeof(count));

    for (const auto& sms : smsMessages) {

        file.write(reinterpret_cast<const char*>(&sms.from.value), sizeof(sms.from.value));
        file.write(reinterpret_cast<const char*>(&sms.to.value), sizeof(sms.to.value));
        
        size_t textLength = sms.text.length();
        file.write(reinterpret_cast<const char*>(&textLength), sizeof(textLength));
        file.write(sms.text.data(), static_cast<std::streamsize>(textLength));
        
        file.write(reinterpret_cast<const char*>(&sms.isRead), sizeof(sms.isRead));
        file.write(reinterpret_cast<const char*>(&sms.isSent), sizeof(sms.isSent));
    }
}

void SmsDb::loadFromFile()
{
    if (!ownNumber.isValid()) {
        return;
    }
    
    std::ifstream file(getStorageFilePath(), std::ios::binary);
    if (!file) {
        throw std::runtime_error("Failed to open SMS database file for reading");
    }
    
    smsMessages.clear();

    size_t count;
    file.read(reinterpret_cast<char*>(&count), sizeof(count));

    for (size_t i = 0; i < count; ++i) {
        Sms sms;

        file.read(reinterpret_cast<char*>(&sms.from.value), sizeof(sms.from.value));
        file.read(reinterpret_cast<char*>(&sms.to.value), sizeof(sms.to.value));
        
        size_t textLength;
        file.read(reinterpret_cast<char*>(&textLength), sizeof(textLength));
        
        sms.text.resize(textLength);
        file.read(&sms.text[0], static_cast<std::streamsize>(textLength));
        
        file.read(reinterpret_cast<char*>(&sms.isRead), sizeof(sms.isRead));
        file.read(reinterpret_cast<char*>(&sms.isSent), sizeof(sms.isSent));
        
        smsMessages.push_back(sms);
    }
}

}
