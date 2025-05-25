#include <gmock/gmock.h>
#include <gtest/gtest.h>

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

TEST_F(TimerPortTestSuite, shallCallTimeoutAfterTimer)
{
    EXPECT_CALL(handlerMock, handleTimeout()).Times(1);
    objectUnderTest.startTimer(std::chrono::milliseconds(50));
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

TEST_F(TimerPortTestSuite, shallNotCallTimeoutIfStopped)
{
    EXPECT_CALL(handlerMock, handleTimeout()).Times(0);
    objectUnderTest.startTimer(std::chrono::milliseconds(50));
    objectUnderTest.stopTimer();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

TEST_F(TimerPortTestSuite, shallCallRedirectAfterTimer)
{
    EXPECT_CALL(handlerMock, handleRedirect()).Times(1);
    objectUnderTest.startRedirectTimer(std::chrono::milliseconds(50));
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

}
