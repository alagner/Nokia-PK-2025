#pragma once

#include <gmock/gmock.h>
#include "ISmsDb.hpp"

namespace ue
{

class ISmsDbMock : public ISmsDb
{
public:
    ISmsDbMock();
    ~ISmsDbMock() override;

    MOCK_METHOD(void, addSms, (common::PhoneNumber from, const std::string& text), (override));
    MOCK_METHOD(void, addSentSms, (common::PhoneNumber from, common::PhoneNumber to, const std::string& text), (override));
    MOCK_METHOD(const std::vector<Sms>&, getAllSms, (), (const, override));
    MOCK_METHOD(unsigned int, getUnreadCount, (), (const, override));
    MOCK_METHOD(void, markAsRead, (size_t index), (override));
    MOCK_METHOD(bool, hasUnreadSms, (), (const, override));
    MOCK_METHOD(void, saveToFile, (), (const, override));
    MOCK_METHOD(void, loadFromFile, (), (override));
    MOCK_METHOD(void, clearMessages, (), (override));
};

} // namespace ue