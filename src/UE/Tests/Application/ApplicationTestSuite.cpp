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
    const common::PhoneNumber PHONE_NUMBER{112};
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
}