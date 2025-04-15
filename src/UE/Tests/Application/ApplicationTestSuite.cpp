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

struct ApplicationConnectedTestSuite : public ApplicationTestSuite
{
    const common::PhoneNumber CALLER_NUMBER{111};
    const common::PhoneNumber CALLE_NUMBER{112};

    ApplicationConnectedTestSuite()
    {
        EXPECT_CALL(timerPortMock, stopTimer());
        EXPECT_CALL(userPortMock, showConnected());

        objectUnderTest.handleAttachAccept();

        testing::Mock::VerifyAndClearExpectations(&timerPortMock);
        testing::Mock::VerifyAndClearExpectations(&userPortMock);
        testing::Mock::VerifyAndClearExpectations(&btsPortMock);
    }
};


TEST_F(ApplicationConnectedTestSuite, shallReceiveIncomingCallAndUserAccepts)
{
    EXPECT_CALL(userPortMock, showIncomingCall(CALLER_NUMBER));
    EXPECT_CALL(timerPortMock, startTimer(std::chrono::milliseconds(30000)));  // 30s from documentation

    objectUnderTest.handleCallRequest(CALLER_NUMBER);
    testing::Mock::VerifyAndClearExpectations(&userPortMock);
    testing::Mock::VerifyAndClearExpectations(&timerPortMock);

    EXPECT_CALL(timerPortMock, stopTimer());
    EXPECT_CALL(btsPortMock, sendCallAccepted(CALLER_NUMBER)); // send call accpeted
    EXPECT_CALL(userPortMock, showDialing()); // talking state

    objectUnderTest.handleUserAction("ACCEPT");
}

TEST_F(ApplicationConnectedTestSuite, shallReceiveIncomingCallAndUserRejects)
{
    EXPECT_CALL(userPortMock, showIncomingCall(CALLER_NUMBER));
    EXPECT_CALL(timerPortMock, startTimer(std::chrono::milliseconds(30000)));
    objectUnderTest.handleCallRequest(CALLER_NUMBER);
    testing::Mock::VerifyAndClearExpectations(&userPortMock);

    EXPECT_CALL(timerPortMock, stopTimer());
    EXPECT_CALL(btsPortMock, sendCallDropped(CALLER_NUMBER)); // senc call dropped
    EXPECT_CALL(userPortMock, showConnected());

    objectUnderTest.handleUserAction("REJECT");
}


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
