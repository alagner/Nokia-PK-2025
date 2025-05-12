#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "Messages/PhoneNumber.hpp"
#include "Application/Models/SmsDatabase.hpp"

namespace ue {

    class SmsTestSuite : public ::testing::Test {
    protected:
        SmsDatabase db;
        common::PhoneNumber sender{101};
        std::string messageA = "First message";
        std::string messageB = "Second message";
    };

    TEST_F(SmsTestSuite, storesSmsCorrectly)
    {
        auto index = db.addSms(sender, messageA);

        EXPECT_EQ(index, 0u) << "First message should be stored at index 0";
        const auto& messages = db.getAllSms();
        EXPECT_EQ(messages.size(), 1u) << "Database should contain exactly one message";
        EXPECT_EQ(messages[0].from, sender);
        EXPECT_EQ(messages[0].text, messageA);
        EXPECT_FALSE(messages[0].isRead) << "Message should be marked as unread initially";
    }

    TEST_F(SmsTestSuite, updatesReadStatusProperly)
    {
        db.addSms(sender, messageA);
        db.addSms(sender, messageB);

        bool success = db.markAsRead(0);
        EXPECT_TRUE(success);
        EXPECT_TRUE(db.getAllSms()[0].isRead);

        EXPECT_FALSE(db.markAsRead(5)) << "Marking invalid index should fail";

        EXPECT_TRUE(db.markAsRead(1));
        EXPECT_TRUE(db.getAllSms()[1].isRead);
    }

    TEST_F(SmsTestSuite, countsUnreadMessagesAccurately)
    {
        EXPECT_EQ(db.getUnreadCount(), 0u) << "Empty DB should have 0 unread";

        db.addSms(sender, messageA);
        db.addSms(sender, messageB);

        EXPECT_EQ(db.getUnreadCount(), 2u);

        db.markAsRead(0);
        EXPECT_EQ(db.getUnreadCount(), 1u);

        db.markAsRead(1);
        EXPECT_EQ(db.getUnreadCount(), 0u);
    }

}
