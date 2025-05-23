#pragma once

#include "ISmsDb.hpp"
#include "Messages/PhoneNumber.hpp"
#include <vector>
#include <string>
#include <fstream>
#include <filesystem>

namespace ue
{

class Sms
{
public:
    common::PhoneNumber from;
    common::PhoneNumber to;
    std::string text;
    bool isRead;
    bool isSent;

    Sms(common::PhoneNumber from, std::string text, bool isRead = false)
        : from(from), to{}, text(text), isRead(isRead), isSent(false) {}

    Sms(common::PhoneNumber from, common::PhoneNumber to, std::string text)
        : from(from), to(to), text(text), isRead(true), isSent(true) {}

    Sms() = default;

    bool operator==(const Sms& other) const {
        return from == other.from &&
               to == other.to &&
               text == other.text &&
               isRead == other.isRead &&
               isSent == other.isSent;
    }
};

class SmsDb : public ISmsDb
{
public:
    SmsDb();
    SmsDb(const common::PhoneNumber& phoneNumber);
    ~SmsDb();


    void addSms(common::PhoneNumber from, const std::string& text) override;
    
    void addSentSms(common::PhoneNumber from, common::PhoneNumber to, const std::string& text) override;

    const std::vector<Sms>& getAllSms() const override;
    
    unsigned int getUnreadCount() const override;
    
    void markAsRead(size_t index) override;
    
    bool hasUnreadSms() const override;
    
    void saveToFile() const override;
    void loadFromFile() override;

    void clearMessages() override { smsMessages.clear(); }

private:
    std::vector<Sms> smsMessages;
    std::string getStorageFilePath() const;
    static constexpr const char* SMS_DB_FILENAME = "sms_database.dat";
    common::PhoneNumber ownNumber{common::PhoneNumber::INVALID_VALUE};
};

}
