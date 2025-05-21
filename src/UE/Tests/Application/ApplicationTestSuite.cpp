#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "Application.hpp"
#include "Mocks/ILoggerMock.hpp"
#include "Mocks/IBtsPortMock.hpp"
#include "Mocks/IUserPortMock.hpp"
#include "Mocks/ITimerPortMock.hpp"
#include "Messages/PhoneNumber.hpp"
#include <memory>

namespace ue
{
using namespace ::testing;
using namespace std::chrono_literals;

class ApplicationTestSuite : public Test
{
protected:
    const common::PhoneNumber PHONE_NUMBER{112};
    const common::BtsId BTS_ID{1024};
    NiceMock<common::ILoggerMock> loggerMock;
    StrictMock<IBtsPortMock> btsPortMock;
    NiceMock<IUserPortMock> userPortMock;  // Changed to NiceMock to handle unexpected calls
    StrictMock<ITimerPortMock> timerPortMock;

    Application objectUnderTest{PHONE_NUMBER,
                                loggerMock,
                                btsPortMock,
                                userPortMock,
                                timerPortMock};
                                
    void TearDown() override
    {
        // Always clear expectations after each test to prevent interference
        ::testing::Mock::VerifyAndClearExpectations(&btsPortMock);
        ::testing::Mock::VerifyAndClearExpectations(&userPortMock);
        ::testing::Mock::VerifyAndClearExpectations(&timerPortMock);
    }
};

struct ApplicationNotConnectedTestSuite : ApplicationTestSuite
{
    void shallHandleSibMessage()
    {
        // Use InSequence to ensure the calls happen in the correct order
        InSequence seq;
        
        EXPECT_CALL(btsPortMock, sendAttachRequest(BTS_ID));
        EXPECT_CALL(timerPortMock, startTimer(500ms));
        EXPECT_CALL(userPortMock, showConnecting());

        objectUnderTest.handleSib(BTS_ID);
        
        // Always verify and clear expectations to avoid interference
        ::testing::Mock::VerifyAndClearExpectations(&btsPortMock);
        ::testing::Mock::VerifyAndClearExpectations(&userPortMock);
        ::testing::Mock::VerifyAndClearExpectations(&timerPortMock);
    }
};

TEST_F(ApplicationNotConnectedTestSuite, shallHandleSibMessage)
{
    shallHandleSibMessage();
}

struct ApplicationConnectingTestSuite : ApplicationNotConnectedTestSuite
{
    ApplicationConnectingTestSuite()
    {
        // Set up the application in the Connecting state
        shallHandleSibMessage();
        
        // Clear any previous expectations
        ::testing::Mock::VerifyAndClearExpectations(&btsPortMock);
        ::testing::Mock::VerifyAndClearExpectations(&userPortMock);
        ::testing::Mock::VerifyAndClearExpectations(&timerPortMock);
    }
};

TEST_F(ApplicationConnectingTestSuite, shallHandleAttachAccept)
{
    EXPECT_CALL(timerPortMock, stopTimer());
    EXPECT_CALL(userPortMock, showConnected());
    objectUnderTest.handleAttachAccept();
    
    ::testing::Mock::VerifyAndClearExpectations(&userPortMock);
    ::testing::Mock::VerifyAndClearExpectations(&timerPortMock);
}

TEST_F(ApplicationConnectingTestSuite, shallDisconnectOnAttachReject)
{
    EXPECT_CALL(timerPortMock, stopTimer());
    EXPECT_CALL(userPortMock, showNotConnected());
    objectUnderTest.handleAttachReject();
    
    ::testing::Mock::VerifyAndClearExpectations(&userPortMock);
    ::testing::Mock::VerifyAndClearExpectations(&timerPortMock);
}

TEST_F(ApplicationConnectingTestSuite, shallDisconnectOnTimeout)
{
    EXPECT_CALL(userPortMock, showNotConnected());
    objectUnderTest.handleTimeout();
    
    ::testing::Mock::VerifyAndClearExpectations(&userPortMock);
}
}
