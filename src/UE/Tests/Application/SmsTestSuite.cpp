#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "Messages/PhoneNumber.hpp"
#include "Application/Models/SmsDatabase.hpp"

namespace ue {

    class SmsTestSuite : public ::testing::Test {
    protected:
        SmsDatabase db;
        common::PhoneNumber sender{101};
        common::PhoneNumber receiver{202};
        std::string messageA = "First message";
        std::string messageB = "Second message";
    };

    TEST_F(SmsTestSuite, storesIncomingSmsCorrectly)
    {
        auto index = db.addIncomingSms(sender, messageA);

        EXPECT_EQ(index, 0u) << "First incoming message should be stored at index 0";
        const auto& messages = db.fetchAllMessages();
        ASSERT_EQ(messages.size(), 1u) << "Database should contain exactly one message";
        EXPECT_EQ(messages[0].contact, sender);
        EXPECT_EQ(messages[0].body, messageA);
        EXPECT_EQ(messages[0].direction, SmsMessage::Direction::Incoming);
        EXPECT_EQ(messages[0].status, SmsMessage::Status::ReceivedUnread);
    }

    TEST_F(SmsTestSuite, storesOutgoingSmsCorrectly)
    {
        auto index = db.addOutgoingSms(receiver, messageB);

        EXPECT_EQ(index, 0u) << "First outgoing message should be stored at index 0";
        const auto& messages = db.fetchAllMessages();
        ASSERT_EQ(messages.size(), 1u) << "Database should contain exactly one message";
        EXPECT_EQ(messages[0].contact, receiver);
        EXPECT_EQ(messages[0].body, messageB);
        EXPECT_EQ(messages[0].direction, SmsMessage::Direction::Outgoing);
        EXPECT_EQ(messages[0].status, SmsMessage::Status::Sent);
    }

    TEST_F(SmsTestSuite, updatesReadStatusProperly)
    {
        db.addIncomingSms(sender, messageA);
        db.addIncomingSms(sender, messageB);

        bool success = db.markAsRead(0);
        EXPECT_TRUE(success);
        EXPECT_EQ(db.fetchAllMessages()[0].status, SmsMessage::Status::ReceivedRead);

        EXPECT_FALSE(db.markAsRead(5)) << "Marking invalid index should fail";

        EXPECT_TRUE(db.markAsRead(1));
        EXPECT_EQ(db.fetchAllMessages()[1].status, SmsMessage::Status::ReceivedRead);
    }

    TEST_F(SmsTestSuite, countsUnreadMessagesAccurately)
    {
        EXPECT_EQ(db.countUnreadMessages(), 0u) << "Empty DB should have 0 unread";

        db.addIncomingSms(sender, messageA);
        db.addIncomingSms(sender, messageB);

        EXPECT_EQ(db.countUnreadMessages(), 2u);

        db.markAsRead(0);
        EXPECT_EQ(db.countUnreadMessages(), 1u);

        db.markAsRead(1);
        EXPECT_EQ(db.countUnreadMessages(), 0u);
    }

    TEST_F(SmsTestSuite, marksLastOutgoingAsFailed)
    {
        db.addOutgoingSms(receiver, messageA);
        bool success = db.markLastOutgoingAsFailed();
        EXPECT_TRUE(success);

        const auto& messages = db.fetchAllMessages();
        ASSERT_EQ(messages.size(), 1u);
        EXPECT_EQ(messages[0].status, SmsMessage::Status::Failed);
    }

    TEST_F(SmsTestSuite, failsToMarkLastOutgoingWhenNoneExists)
    {
        db.addIncomingSms(sender, messageA);
        bool success = db.markLastOutgoingAsFailed();
        EXPECT_FALSE(success) << "No outgoing message to mark as failed";
    }

}
