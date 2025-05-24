#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "SmsDb.hpp"
#include "Mocks/ILoggerMock.hpp"
#include "Messages/PhoneNumber.hpp"
#include <filesystem>
#include <fstream>
#include <algorithm>

namespace ue
{
using namespace ::testing;

class SmsDbTestSuite : public Test
{
protected:
    const common::PhoneNumber SENDER_NUMBER{123};
    const common::PhoneNumber ANOTHER_SENDER{124};
    const common::PhoneNumber OWN_NUMBER{111};
    const std::string SMS_TEXT{"Hello1"};
    const std::string ANOTHER_SMS_TEXT{"Hello2"};
    const std::string TEST_DB_PATH{"test_sms_database.dat"};
    
    SmsDb objectUnderTest{OWN_NUMBER};

    void SetUp() override
    {
        if (std::filesystem::exists(TEST_DB_PATH))
        {
            std::filesystem::remove(TEST_DB_PATH);
        }
        objectUnderTest.clearMessages();
    }

    void TearDown() override
    {
        if (std::filesystem::exists(TEST_DB_PATH))
        {
            std::filesystem::remove(TEST_DB_PATH);
        }
    }
};

TEST_F(SmsDbTestSuite, shallStoreSmsMessage)
{
    objectUnderTest.addSms(SENDER_NUMBER, SMS_TEXT);
    
    // Verify the message was stored
    auto messages = objectUnderTest.getAllSms();
    EXPECT_EQ(1, messages.size());
    EXPECT_EQ(SENDER_NUMBER, messages[0].from);
    EXPECT_EQ(SMS_TEXT, messages[0].text);
    EXPECT_FALSE(messages[0].isRead);
    EXPECT_FALSE(messages[0].isSent);
}

TEST_F(SmsDbTestSuite, shallStoreMultipleSmsMessages)
{
    objectUnderTest.addSms(SENDER_NUMBER, SMS_TEXT);
    objectUnderTest.addSms(ANOTHER_SENDER, ANOTHER_SMS_TEXT);
    
    // Verify both messages were stored
    auto messages = objectUnderTest.getAllSms();
    EXPECT_EQ(2, messages.size());
    
    // Check first message
    EXPECT_EQ(SENDER_NUMBER, messages[0].from);
    EXPECT_EQ(SMS_TEXT, messages[0].text);
    EXPECT_FALSE(messages[0].isRead);
    
    // Check second message
    EXPECT_EQ(ANOTHER_SENDER, messages[1].from);
    EXPECT_EQ(ANOTHER_SMS_TEXT, messages[1].text);
    EXPECT_FALSE(messages[1].isRead);
}

TEST_F(SmsDbTestSuite, shallMarkSmsAsRead)
{
    objectUnderTest.addSms(SENDER_NUMBER, SMS_TEXT);
    
    // Verify unread
    auto messages = objectUnderTest.getAllSms();
    ASSERT_EQ(1, messages.size());
    EXPECT_FALSE(messages[0].isRead);
    
    // Mark as read
    objectUnderTest.markAsRead(0);
    
    // Verify it's marked as read
    messages = objectUnderTest.getAllSms();
    EXPECT_TRUE(messages[0].isRead);
}

TEST_F(SmsDbTestSuite, shallTrackUnreadSmsCount)
{
    EXPECT_EQ(0, objectUnderTest.getUnreadCount());
    EXPECT_FALSE(objectUnderTest.hasUnreadSms());
    
    objectUnderTest.addSms(SENDER_NUMBER, SMS_TEXT);
    EXPECT_EQ(1, objectUnderTest.getUnreadCount());
    EXPECT_TRUE(objectUnderTest.hasUnreadSms());
    
    objectUnderTest.addSms(ANOTHER_SENDER, ANOTHER_SMS_TEXT);
    EXPECT_EQ(2, objectUnderTest.getUnreadCount());
    EXPECT_TRUE(objectUnderTest.hasUnreadSms());
    
    // Mark first message as read
    objectUnderTest.markAsRead(0);
    EXPECT_EQ(1, objectUnderTest.getUnreadCount());
    EXPECT_TRUE(objectUnderTest.hasUnreadSms());
    
    // Mark second message as read
    objectUnderTest.markAsRead(1);
    EXPECT_EQ(0, objectUnderTest.getUnreadCount());
    EXPECT_FALSE(objectUnderTest.hasUnreadSms());
}

TEST_F(SmsDbTestSuite, shallRetrieveAllSmsMessages)
{
    objectUnderTest.addSms(SENDER_NUMBER, SMS_TEXT);
    objectUnderTest.addSms(ANOTHER_SENDER, ANOTHER_SMS_TEXT);
    
    // Retrieve all messages
    auto messages = objectUnderTest.getAllSms();
    
    // Verify all messages are returned
    EXPECT_EQ(2, messages.size());
    
    bool foundFirstMessage = false;
    bool foundSecondMessage = false;
    
    for (const auto& msg : messages)
    {
        if (msg.from == SENDER_NUMBER && msg.text == SMS_TEXT)
        {
            foundFirstMessage = true;
        }
        else if (msg.from == ANOTHER_SENDER && msg.text == ANOTHER_SMS_TEXT)
        {
            foundSecondMessage = true;
        }
    }
    
    EXPECT_TRUE(foundFirstMessage);
    EXPECT_TRUE(foundSecondMessage);
}

TEST_F(SmsDbTestSuite, shallHandleEmptyDatabase)
{
    auto messages = objectUnderTest.getAllSms();
    EXPECT_TRUE(messages.empty());
    EXPECT_EQ(0, objectUnderTest.getUnreadCount());
    EXPECT_FALSE(objectUnderTest.hasUnreadSms());
}

TEST_F(SmsDbTestSuite, shallHandleMarkingNonExistentMessageAsRead)
{
    ASSERT_NO_THROW(objectUnderTest.markAsRead(999));
    
    // Unread count should remain 0
    EXPECT_EQ(0, objectUnderTest.getUnreadCount());
    EXPECT_FALSE(objectUnderTest.hasUnreadSms());
}

TEST_F(SmsDbTestSuite, shallStoreEmptySmsText)
{
    const std::string EMPTY_TEXT{""};
    
    // Store SMS with empty text
    objectUnderTest.addSms(SENDER_NUMBER, EMPTY_TEXT);
    
    // Verify it was stored
    auto messages = objectUnderTest.getAllSms();
    EXPECT_EQ(1, messages.size());
    EXPECT_EQ(EMPTY_TEXT, messages[0].text);
}

TEST_F(SmsDbTestSuite, shallHandleLongSmsText)
{
    const std::string LONG_TEXT(1000, 'A');
    
    objectUnderTest.addSms(SENDER_NUMBER, LONG_TEXT);
    
    // Verify it was stored correctly
    auto messages = objectUnderTest.getAllSms();
    EXPECT_EQ(1, messages.size());
    EXPECT_EQ(LONG_TEXT, messages[0].text);
}

TEST_F(SmsDbTestSuite, shallStoreSentSmsMessages)
{
    const common::PhoneNumber TO_NUMBER{125};
    
    objectUnderTest.addSentSms(OWN_NUMBER, TO_NUMBER, SMS_TEXT);
    
    // Verify sent SMS properties
    auto messages = objectUnderTest.getAllSms();
    EXPECT_EQ(1, messages.size());
    EXPECT_EQ(OWN_NUMBER, messages[0].from);
    EXPECT_EQ(TO_NUMBER, messages[0].to);
    EXPECT_EQ(SMS_TEXT, messages[0].text);
    EXPECT_TRUE(messages[0].isRead); 
    EXPECT_TRUE(messages[0].isSent);
}

TEST_F(SmsDbTestSuite, shallDifferentiateBetweenSentAndReceivedMessages)
{
    const common::PhoneNumber TO_NUMBER{125};
    
    objectUnderTest.addSms(SENDER_NUMBER, SMS_TEXT);
    objectUnderTest.addSentSms(OWN_NUMBER, TO_NUMBER, ANOTHER_SMS_TEXT);
    
    auto messages = objectUnderTest.getAllSms();
    EXPECT_EQ(2, messages.size());
    
    // Find received SMS
    auto receivedSms = std::find_if(messages.begin(), messages.end(), 
        [](const Sms& sms) { return !sms.isSent; });
    ASSERT_NE(receivedSms, messages.end());
    EXPECT_FALSE(receivedSms->isRead);
    EXPECT_FALSE(receivedSms->isSent);
    
    // Find sent SMS
    auto sentSms = std::find_if(messages.begin(), messages.end(), 
        [](const Sms& sms) { return sms.isSent; });
    ASSERT_NE(sentSms, messages.end());
    EXPECT_TRUE(sentSms->isRead);
    EXPECT_TRUE(sentSms->isSent);
}

TEST_F(SmsDbTestSuite, shallClearAllMessages)
{
    objectUnderTest.addSms(SENDER_NUMBER, SMS_TEXT);
    objectUnderTest.addSms(ANOTHER_SENDER, ANOTHER_SMS_TEXT);
    
    // Verify messages exist
    EXPECT_EQ(2, objectUnderTest.getAllSms().size());
    EXPECT_EQ(2, objectUnderTest.getUnreadCount());
    
    // Clear messages
    objectUnderTest.clearMessages();
    
    // Verify all messages are cleared
    EXPECT_TRUE(objectUnderTest.getAllSms().empty());
    EXPECT_EQ(0, objectUnderTest.getUnreadCount());
    EXPECT_FALSE(objectUnderTest.hasUnreadSms());
}

TEST_F(SmsDbTestSuite, shallCountOnlyUnreadReceivedMessages)
{
    const common::PhoneNumber TO_NUMBER{125};
    
    // Add received SMS (unread)
    objectUnderTest.addSms(SENDER_NUMBER, SMS_TEXT);
    
    // Add sent SMS (automatically read)
    objectUnderTest.addSentSms(OWN_NUMBER, TO_NUMBER, ANOTHER_SMS_TEXT);
    
    // Only the received SMS should count as unread
    EXPECT_EQ(1, objectUnderTest.getUnreadCount());
    EXPECT_TRUE(objectUnderTest.hasUnreadSms());
    
    // Mark received SMS as read
    objectUnderTest.markAsRead(0);
    
    // Now no unread messages
    EXPECT_EQ(0, objectUnderTest.getUnreadCount());
    EXPECT_FALSE(objectUnderTest.hasUnreadSms());
}

TEST_F(SmsDbTestSuite, shallHandleInvalidIndexForMarkAsRead)
{
    // Add one message
    objectUnderTest.addSms(SENDER_NUMBER, SMS_TEXT);
    
    ASSERT_NO_THROW(objectUnderTest.markAsRead(1));
    ASSERT_NO_THROW(objectUnderTest.markAsRead(999));
    
    // Original message should remain unread
    auto messages = objectUnderTest.getAllSms();
    EXPECT_EQ(1, messages.size());
    EXPECT_FALSE(messages[0].isRead);
}

}