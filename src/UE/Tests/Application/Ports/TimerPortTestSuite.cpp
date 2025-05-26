#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <thread>
#include <chrono>

#include "Ports/TimerPort.hpp"
#include "Mocks/ILoggerMock.hpp"
#include "Mocks/ITimerPortMock.hpp"
#include "Messages/PhoneNumber.hpp"

namespace ue
{
using namespace ::testing;

class TimerPortTestSuite : public Test
{
protected:
    const common::PhoneNumber PHONE_NUMBER{112};
    const common::PhoneNumber RECIPIENT_NUMBER{113};
    const common::PhoneNumber SENDER_NUMBER{114};
    const std::string SMS_TEXT = "Hello1";
    const std::chrono::seconds SMS_NOTIFICATION_TIMEOUT{5};
    const std::chrono::seconds CALL_TIMEOUT{30};

    NiceMock<common::ILoggerMock> loggerMock;
    StrictMock<ITimerEventsHandlerMock> handlerMock;

    
    TimerPort objectUnderTest{loggerMock};

    TimerPortTestSuite()
    {
        objectUnderTest.start(handlerMock);
    }
    
    ~TimerPortTestSuite()
    {
        objectUnderTest.stop();
    }
};

TEST_F(TimerPortTestSuite, shallStart)
{
}

TEST_F(TimerPortTestSuite, shallStopPreviousTimerWhenStartingNewOne)
{
    objectUnderTest.startTimer(std::chrono::seconds(10));
    objectUnderTest.startTimer(std::chrono::seconds(20));
    objectUnderTest.stopTimer();
}

TEST_F(TimerPortTestSuite, shallHandleTimeoutForIncomingSmsNotification)
{
    EXPECT_CALL(handlerMock, handleTimeout());
    objectUnderTest.startTimer(SMS_NOTIFICATION_TIMEOUT);
    
    handlerMock.handleTimeout();
}

TEST_F(TimerPortTestSuite, shallStopTimerWhenInterruptingSmsSend)
{
    objectUnderTest.startTimer(std::chrono::seconds(60));
    objectUnderTest.stopTimer();
}

TEST_F(TimerPortTestSuite, shallHandleTimeoutDuringSmsSending)
{
    EXPECT_CALL(handlerMock, handleTimeout());
    objectUnderTest.startTimer(std::chrono::seconds(10));
    
    handlerMock.handleTimeout();
}

TEST_F(TimerPortTestSuite, shallHandleTimeoutDuringIncomingCallWhileViewingSms)
{
    EXPECT_CALL(handlerMock, handleTimeout());
    objectUnderTest.startTimer(CALL_TIMEOUT);
    
    handlerMock.handleTimeout();
}

TEST_F(TimerPortTestSuite, shallHandleTimeoutDuringIncomingCallWhileComposingSms)
{
    EXPECT_CALL(handlerMock, handleTimeout());
    objectUnderTest.startTimer(CALL_TIMEOUT);
    
    handlerMock.handleTimeout();
}

TEST_F(TimerPortTestSuite, shallCancelTimeoutWhenAcceptingCallDuringSmsSend)
{
    objectUnderTest.startTimer(CALL_TIMEOUT);
    objectUnderTest.stopTimer();
}

TEST_F(TimerPortTestSuite, shallCancelTimeoutWhenRejectingCallDuringSmsSend)
{
    objectUnderTest.startTimer(CALL_TIMEOUT);
    objectUnderTest.stopTimer();
}

TEST_F(TimerPortTestSuite, shallHandleUnknownRecipientTimeoutWhenSendingSms)
{
    EXPECT_CALL(handlerMock, handleTimeout());
    objectUnderTest.startTimer(std::chrono::seconds(5));

    handlerMock.handleTimeout();
}

}