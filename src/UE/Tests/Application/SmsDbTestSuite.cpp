#include <gtest/gtest.h>
#include "Messages/PhoneNumber.hpp"
#include "Application/SmsDb.hpp"

namespace ue
{

class SmsDbTestSuite : public ::testing::Test
{
protected:
    SmsDb smsDb;
    common::PhoneNumber testSender{100};
    std::string testMessage1 = "Hello, this is a test SMS";
    std::string testMessage2 = "Another test message";
};

TEST_F(SmsDbTestSuite, shouldStoreNewSms)
{
    std::size_t index = smsDb.addSms(testSender, testMessage1);
    ASSERT_EQ(0u, index);
    ASSERT_EQ(1u, smsDb.getAllSms().size());
    ASSERT_EQ(testSender, smsDb.getAllSms()[0].from);
    ASSERT_EQ(testMessage1, smsDb.getAllSms()[0].text);
    ASSERT_FALSE(smsDb.getAllSms()[0].isRead);
}

TEST_F(SmsDbTestSuite, shouldCountUnreadSms)
{
    smsDb.addSms(testSender, testMessage1);
    ASSERT_EQ(1u, smsDb.getUnreadCount());
    
    smsDb.addSms(testSender, testMessage2);
    ASSERT_EQ(2u, smsDb.getUnreadCount());
    
    smsDb.markAsRead(0);
    ASSERT_EQ(1u, smsDb.getUnreadCount());
    
    smsDb.markAsRead(1);
    ASSERT_EQ(0u, smsDb.getUnreadCount());
}

TEST_F(SmsDbTestSuite, shouldMarkSmsAsRead)
{
    smsDb.addSms(testSender, testMessage1);
    ASSERT_FALSE(smsDb.getAllSms()[0].isRead);
    
    bool result = smsDb.markAsRead(0);
    ASSERT_TRUE(result);
    ASSERT_TRUE(smsDb.getAllSms()[0].isRead);
    
    // Invalid index
    result = smsDb.markAsRead(1);
    ASSERT_FALSE(result);
}

} // namespace ue
