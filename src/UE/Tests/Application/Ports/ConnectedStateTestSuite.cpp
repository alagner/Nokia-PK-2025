#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "States/ConnectedState.hpp"
#include "Mocks/IUserPortMock.hpp"
#include "Mocks/ILoggerMock.hpp"
#include "Mocks/IBtsPortMock.hpp"
#include "Mocks/ITimerPortMock.hpp"

namespace ue
{
using namespace ::testing;

class ConnectedStateTestSuite : public Test
{
protected:
    StrictMock<common::ILoggerMock> loggerMock;
    StrictMock<IUserPortMock> userMock;
    StrictMock<IBtsPortMock> btsPortMock;
    StrictMock<ITimerPortMock> timerPortMock;

    Context context{loggerMock, btsPortMock, userMock, timerPortMock};
    std::unique_ptr<ConnectedState> state;

    void SetUp() override
    {
        EXPECT_CALL(userMock, showConnected());
        state = std::make_unique<ConnectedState>(context);
        context.setState<ConnectedState>();

    }
};

TEST_F(ConnectedStateTestSuite, shallNotifyUserOnSms)
{
    const common::PhoneNumber FROM{123};
    const std::string TEXT = "Test SMS";

    EXPECT_CALL(loggerMock, log(common::ILogger::INFO_LEVEL, "[ConnectedState] Received SMS from: 123, text: Test SMS")).Times(1);
    EXPECT_CALL(userMock, showNewSms()).Times(1);

    context.state->handleSms(FROM, TEXT);
}





}
