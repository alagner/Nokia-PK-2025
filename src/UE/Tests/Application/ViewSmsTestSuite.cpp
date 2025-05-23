#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "Application.hpp"
#include "Mocks/ILoggerMock.hpp"
#include "Mocks/IBtsPortMock.hpp"
#include "Mocks/IUserPortMock.hpp"
#include "Mocks/ITimerPortMock.hpp"
#include "Messages/PhoneNumber.hpp"
#include "Messages/BtsId.hpp"
#include <memory>

namespace ue
{
using namespace ::testing;

class ApplicationTestSuite : public Test
{
protected:
    const common::PhoneNumber PHONE_NUMBER{42}; // test specific number, don't use in production
    NiceMock<common::ILoggerMock> loggerMock;
    NiceMock<IBtsPortMock> btsPortMock;
    NiceMock<IUserPortMock> userPortMock;
    NiceMock<ITimerPortMock> timerPortMock;

    Application objectUnderTest{PHONE_NUMBER,
                                loggerMock,
                                btsPortMock,
                                userPortMock,
                                timerPortMock};
};

struct ViewSmsTestSuite : ApplicationTestSuite 
{
protected:
    void SetUp() override
    {
        // Clear SMS database - without this tests will fail
        auto& smsDb = const_cast<SmsDb&>(objectUnderTest.getSmsDb());
        smsDb.clearMessages();

        // Start in Connected state
        EXPECT_CALL(userPortMock, showConnecting());
        EXPECT_CALL(btsPortMock, sendAttachRequest(common::BtsId{1}));
        objectUnderTest.handleSib(common::BtsId{1});
        
        EXPECT_CALL(userPortMock, showConnected());
        objectUnderTest.handleAttachAccept();
    }
};

TEST_F(ViewSmsTestSuite, ShouldDisplaySmsListWhenViewingSms)
{
    InSequence seq;

    EXPECT_CALL(timerPortMock, startTimer(std::chrono::milliseconds{500})).Times(AtLeast(0));
    EXPECT_CALL(timerPortMock, stopTimer()).Times(AtLeast(0));
    EXPECT_CALL(userPortMock, clearIncomingCallText()).Times(AtLeast(0));
    EXPECT_CALL(userPortMock, clearOutgoingCallText()).Times(AtLeast(0));
    EXPECT_CALL(userPortMock, showNewSms(false)).Times(AtLeast(0));
    
    std::vector<Sms> emptySmsDb;
    EXPECT_CALL(userPortMock, setSmsList(emptySmsDb));
    EXPECT_CALL(userPortMock, setSelectSmsCallback(_));
    EXPECT_CALL(userPortMock, showSmsList());

    objectUnderTest.viewSms();
}

TEST_F(ViewSmsTestSuite, ShouldDisplaySmsContentWhenSelectingSms)
{
    const common::PhoneNumber senderNumber{123};
    const std::string smsText = "Test message";
    objectUnderTest.handleSms(senderNumber, smsText);

    // when accessing SMS list, expect the list to be shown with our test message
    std::vector<Sms> smsDbWithOneMessage = {
        Sms{senderNumber, smsText}
    };
    EXPECT_CALL(userPortMock, setSmsList(smsDbWithOneMessage));
    EXPECT_CALL(userPortMock, setSelectSmsCallback(_));
    EXPECT_CALL(userPortMock, showSmsList());
    objectUnderTest.viewSms();

    EXPECT_CALL(userPortMock, showSmsContent(std::to_string(senderNumber.value), smsText));
    objectUnderTest.selectSms(0);
}

TEST_F(ViewSmsTestSuite, ShouldMarkSmsAsReadWhenViewing)
{
    const common::PhoneNumber senderNumber{123};
    const std::string smsText = "Test message";
    objectUnderTest.handleSms(senderNumber, smsText);

    std::vector<Sms> smsDbWithOneUnreadMessage = {
        Sms{senderNumber, smsText, false}
    };
    EXPECT_CALL(userPortMock, setSmsList(smsDbWithOneUnreadMessage));
    EXPECT_CALL(userPortMock, setSelectSmsCallback(_));
    EXPECT_CALL(userPortMock, showSmsList());
    objectUnderTest.viewSms();

    // When selecting the SMS, its content should be shown
    EXPECT_CALL(userPortMock, showSmsContent(std::to_string(senderNumber.value), smsText));
    objectUnderTest.selectSms(0);

    // Verify that SMS has been marked as read by checking if any unread messages remain
    const auto& smsDb = objectUnderTest.getSmsDb();
    EXPECT_FALSE(smsDb.hasUnreadSms());
}

TEST_F(ViewSmsTestSuite, ShouldHandleEmptySmsListCorrectly)
{
    // The SMS list should be empty - it's been cleared in SetUp method

    std::vector<Sms> emptySmsDb;
    EXPECT_CALL(userPortMock, setSmsList(emptySmsDb)).Times(AtLeast(1));
    EXPECT_CALL(userPortMock, setSelectSmsCallback(_)).Times(AtLeast(1));
    EXPECT_CALL(userPortMock, showSmsList()).Times(AtLeast(1));
    objectUnderTest.viewSms();

    // Verify that selecting a message from empty list is handled properly
    objectUnderTest.selectSms(0);

    // Return to main menu
    EXPECT_CALL(userPortMock, showConnected());
    objectUnderTest.closeSmsView();
}

TEST_F(ViewSmsTestSuite, ShouldAllowViewingMultipleSmsInSequence)
{
    const common::PhoneNumber sender1{43};
    const common::PhoneNumber sender2{44};
    const std::string text1 = "Hello1";
    const std::string text2 = "Hello2";
    objectUnderTest.handleSms(sender1, text1);
    objectUnderTest.handleSms(sender2, text2);

    // Compare only relevant fields of SMSes
    EXPECT_CALL(userPortMock, setSmsList(Truly([&](const std::vector<Sms>& smsList) -> bool {
        return smsList.size() == 2 && 
               smsList[0].from == sender1 && 
               smsList[0].text == text1 &&
               smsList[1].from == sender2 && 
               smsList[1].text == text2;
    })));
    EXPECT_CALL(userPortMock, setSelectSmsCallback(_));
    EXPECT_CALL(userPortMock, showSmsList());
    objectUnderTest.viewSms();

    // View first message
    EXPECT_CALL(userPortMock, showSmsContent(std::to_string(sender1.value), text1));
    objectUnderTest.selectSms(0);

    EXPECT_CALL(userPortMock, showConnected());
    objectUnderTest.closeSmsView();  // Return to main menu
    
    // Compare only relevant fields of SMSes
    EXPECT_CALL(userPortMock, setSmsList(Truly([&](const std::vector<Sms>& smsList) -> bool {
        return smsList.size() == 2 && 
               smsList[0].from == sender1 && 
               smsList[0].text == text1 &&
               smsList[1].from == sender2 && 
               smsList[1].text == text2;
    })));
    EXPECT_CALL(userPortMock, setSelectSmsCallback(_));
    EXPECT_CALL(userPortMock, showSmsList());
    objectUnderTest.viewSms();
    
    // View second message
    EXPECT_CALL(userPortMock, showSmsContent(std::to_string(sender2.value), text2));
    objectUnderTest.selectSms(1);

    // Return to main menu
    EXPECT_CALL(userPortMock, showConnected());
    objectUnderTest.closeSmsView();
}

TEST_F(ViewSmsTestSuite, ShouldCloseCurrentSmsAndReturnToList)
{
    const common::PhoneNumber senderNumber{43};
    const std::string smsText = "Test message";
    objectUnderTest.handleSms(senderNumber, smsText);

    std::vector<Sms> smsDbWithOneMessage = {
        Sms{senderNumber, smsText}
    };
    EXPECT_CALL(userPortMock, setSmsList(smsDbWithOneMessage));
    EXPECT_CALL(userPortMock, setSelectSmsCallback(_));
    EXPECT_CALL(userPortMock, showSmsList());
    objectUnderTest.viewSms();

    EXPECT_CALL(userPortMock, showSmsContent(std::to_string(senderNumber.value), smsText));
    objectUnderTest.selectSms(0);

    EXPECT_CALL(userPortMock, showConnected());
    objectUnderTest.closeSmsView();
}

TEST_F(ViewSmsTestSuite, ShouldCloseListAndReturnToMainMenu)
{
    std::vector<Sms> emptySmsDb;
    EXPECT_CALL(userPortMock, setSmsList(emptySmsDb));
    EXPECT_CALL(userPortMock, setSelectSmsCallback(_));
    EXPECT_CALL(userPortMock, showSmsList());
    objectUnderTest.viewSms();

    EXPECT_CALL(userPortMock, showConnected());
    objectUnderTest.closeSmsView();
}
}