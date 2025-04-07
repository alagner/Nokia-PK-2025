#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "Application.hpp"
#include "Mocks/ILoggerMock.hpp"
#include "Mocks/IBtsPortMock.hpp"
#include "Mocks/IUserPortMock.hpp"
#include "Mocks/ITimerPortMock.hpp"
#include "Messages/PhoneNumber.hpp"
#include <memory>
#include <chrono> // Include chrono

namespace ue
{
using namespace ::testing;
using namespace std::chrono_literals; // Add namespace for literals like ms

using LoggerNiceMock = NiceMock<common::ILoggerMock>;
using BtsPortStrictMock = StrictMock<IBtsPortMock>;
using UserPortStrictMock = StrictMock<IUserPortMock>;
using TimerPortStrictMock = StrictMock<ITimerPortMock>;


class ApplicationTestSuite : public Test
{
protected:
    const common::PhoneNumber PHONE_NUMBER{112};
    const common::BtsId BTS_ID{1024};
    LoggerNiceMock loggerMock;
    BtsPortStrictMock btsPortMock;
    UserPortStrictMock userPortMock;
    TimerPortStrictMock timerPortMock;

    Application objectUnderTest{PHONE_NUMBER,
                                loggerMock,
                                btsPortMock,
                                userPortMock,
                                timerPortMock};
};

struct ApplicationNotConnectedTestSuite : ApplicationTestSuite
{
    void receiveSibAndEnterConnectingState()
     {
         EXPECT_CALL(btsPortMock, sendAttachRequest(BTS_ID));
         EXPECT_CALL(timerPortMock, startTimer(500ms));
         EXPECT_CALL(userPortMock, showConnecting());
         objectUnderTest.handleSib(BTS_ID);
         Mock::VerifyAndClearExpectations(&btsPortMock);
         Mock::VerifyAndClearExpectations(&timerPortMock);
         Mock::VerifyAndClearExpectations(&userPortMock);
     }
};

TEST_F(ApplicationNotConnectedTestSuite, shallHandleSibMessage)
{
    receiveSibAndEnterConnectingState();
}

struct ApplicationConnectingTestSuite : ApplicationNotConnectedTestSuite
 {
     ApplicationConnectingTestSuite()
     {
         receiveSibAndEnterConnectingState();
     }
 };

TEST_F(ApplicationConnectingTestSuite, shallConnectOnAttachAccept)
{
    EXPECT_CALL(timerPortMock, stopTimer());
    EXPECT_CALL(userPortMock, showConnected());
    EXPECT_CALL(userPortMock, showNewSms(false));

    objectUnderTest.handleAttachAccept();
}

TEST_F(ApplicationConnectingTestSuite, shallDisConnectOnAttachReject)
{
    EXPECT_CALL(timerPortMock, stopTimer());
    EXPECT_CALL(userPortMock, showNotConnected());
    objectUnderTest.handleAttachReject();
}

TEST_F(ApplicationConnectingTestSuite, shallDisConnectOnTimeout)
{
     EXPECT_CALL(timerPortMock, stopTimer());
     EXPECT_CALL(userPortMock, showNotConnected());
     objectUnderTest.handleTimeout();
}

TEST_F(ApplicationConnectingTestSuite, shallDisConnectOnDisconnect)
{
    EXPECT_CALL(timerPortMock, stopTimer());
    EXPECT_CALL(userPortMock, showNotConnected());
    objectUnderTest.handleDisconnect();
}

}