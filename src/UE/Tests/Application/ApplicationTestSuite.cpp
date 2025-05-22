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

struct ApplicationNotConnectedTestSuite : ApplicationTestSuite
{};

TEST_F(ApplicationNotConnectedTestSuite, todo)
{
}

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
}