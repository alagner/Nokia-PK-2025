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
    std::size_t index = smsDb.addReceivedSms(testSender, testMessage1);
    ASSERT_EQ(0u, index);
    ASSERT_EQ(1u, smsDb.getAllSms().size());
    ASSERT_EQ(testSender, smsDb.getAllSms()[0].peer);
    ASSERT_EQ(testMessage1, smsDb.getAllSms()[0].text);
    ASSERT_EQ(SmsMessage::Status::RECEIVED_UNREAD, smsDb.getAllSms()[0].status);
}

TEST_F(SmsDbTestSuite, shouldCountUnreadSms)
{
    smsDb.addReceivedSms(testSender, testMessage1);
    ASSERT_EQ(1u, smsDb.getUnreadCount());
    
    smsDb.addReceivedSms(testSender, testMessage2);
    ASSERT_EQ(2u, smsDb.getUnreadCount());
    
    smsDb.markAsRead(0);
    ASSERT_EQ(1u, smsDb.getUnreadCount());
    
    smsDb.markAsRead(1);
    ASSERT_EQ(0u, smsDb.getUnreadCount());
}

TEST_F(SmsDbTestSuite, shouldMarkSmsAsRead)
{
    smsDb.addReceivedSms(testSender, testMessage1);
    ASSERT_EQ(SmsMessage::Status::RECEIVED_UNREAD, smsDb.getAllSms()[0].status);
    
    bool result = smsDb.markAsRead(0);
    ASSERT_TRUE(result);
    ASSERT_EQ(SmsMessage::Status::RECEIVED_READ, smsDb.getAllSms()[0].status);
    
    // Invalid index
    result = smsDb.markAsRead(1);
    ASSERT_FALSE(result);
}

} // namespace ue
