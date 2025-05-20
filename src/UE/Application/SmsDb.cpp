#include "SmsDb.hpp"
#include <iostream>
#include <stdexcept>

namespace ue
{

SmsDb::SmsDb()
{
    // Default constructor without phone number - will only work 
    // with saveToFile/loadFromFile if ownNumber is set later
}

SmsDb::SmsDb(const common::PhoneNumber& phoneNumber) : ownNumber(phoneNumber)
{
    try {
        loadFromFile();
    } catch (const std::exception& e) {
        // Silently fail if file doesn't exist yet or cannot be read
        // It will be created when saveToFile is called
    }
}

SmsDb::~SmsDb()
{
    try {
        saveToFile();
    } catch (const std::exception& e) {
        // Just log the error, can't throw from destructor
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
    // Get the user's home directory or current directory for storing the SMS database
    const char* homeDir = std::getenv("HOME");
    std::string basePath = homeDir ? homeDir : ".";
    
    // Include phone number in the filename to have different files for different numbers
    if (ownNumber.isValid()) {
        return basePath + "/.ue_" + std::to_string(ownNumber.value) + "_" + SMS_DB_FILENAME;
    } else {
        // Fallback if no phone number is set
        return basePath + "/.ue_" + SMS_DB_FILENAME;
    }
}

void SmsDb::saveToFile() const
{
    if (!ownNumber.isValid()) {
        // Skip saving if no valid phone number is set
        return;
    }
    
    std::ofstream file(getStorageFilePath(), std::ios::binary);
    if (!file) {
        throw std::runtime_error("Failed to open SMS database file for writing");
    }
    
    // Write the number of messages
    size_t count = smsMessages.size();
    file.write(reinterpret_cast<const char*>(&count), sizeof(count));
    
    // Write each message
    for (const auto& sms : smsMessages) {
        // Write phone numbers
        file.write(reinterpret_cast<const char*>(&sms.from.value), sizeof(sms.from.value));
        file.write(reinterpret_cast<const char*>(&sms.to.value), sizeof(sms.to.value));
        
        // Write text 
        size_t textLength = sms.text.length();
        file.write(reinterpret_cast<const char*>(&textLength), sizeof(textLength));
        file.write(sms.text.data(), static_cast<std::streamsize>(textLength));
        
        // Write flags
        file.write(reinterpret_cast<const char*>(&sms.isRead), sizeof(sms.isRead));
        file.write(reinterpret_cast<const char*>(&sms.isSent), sizeof(sms.isSent));
    }
}

void SmsDb::loadFromFile()
{
    if (!ownNumber.isValid()) {
        // Skip loading if no valid phone number is set
        return;
    }
    
    std::ifstream file(getStorageFilePath(), std::ios::binary);
    if (!file) {
        throw std::runtime_error("Failed to open SMS database file for reading");
    }
    
    // Clear current messages
    smsMessages.clear();
    
    // Read the number of messages
    size_t count;
    file.read(reinterpret_cast<char*>(&count), sizeof(count));
    
    // Read each message
    for (size_t i = 0; i < count; ++i) {
        Sms sms;
        
        // Read phone numbers
        file.read(reinterpret_cast<char*>(&sms.from.value), sizeof(sms.from.value));
        file.read(reinterpret_cast<char*>(&sms.to.value), sizeof(sms.to.value));
        
        // Read text
        size_t textLength;
        file.read(reinterpret_cast<char*>(&textLength), sizeof(textLength));
        
        sms.text.resize(textLength);
        file.read(&sms.text[0], static_cast<std::streamsize>(textLength));
        
        // Read flags
        file.read(reinterpret_cast<char*>(&sms.isRead), sizeof(sms.isRead));
        file.read(reinterpret_cast<char*>(&sms.isSent), sizeof(sms.isSent));
        
        // Add to vector
        smsMessages.push_back(sms);
    }
}

}
