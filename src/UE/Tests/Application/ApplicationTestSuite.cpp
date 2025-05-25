#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "Application.hpp"
#include "Mocks/ILoggerMock.hpp"
#include "Mocks/IBtsPortMock.hpp"
#include "Mocks/IUserPortMock.hpp"
#include "Mocks/ITimerPortMock.hpp"
#include "Messages/PhoneNumber.hpp"
#include <memory>

namespace ue {
using namespace ::testing;

class ApplicationTestSuite : public Test {
protected:
    const common::PhoneNumber testPhoneNumber{112};
    NiceMock<common::ILoggerMock> loggerMock;
    StrictMock<IBtsPortMock> btsPortMock;
    StrictMock<IUserPortMock> userPortMock;
    StrictMock<ITimerPortMock> timerPortMock;

    std::unique_ptr<Application> app;

    void SetUp() override {
        EXPECT_CALL(userPortMock, showNotConnected());
    }

    void initApp() {
        app = std::make_unique<Application>(
            testPhoneNumber, loggerMock, btsPortMock, userPortMock, timerPortMock);
    }

    void clearExpectations() {
        Mock::VerifyAndClearExpectations(&userPortMock);
    }
};

TEST_F(ApplicationTestSuite, AttachesToBtsOnSibReception) {
    initApp();
    clearExpectations();

    common::BtsId btsId{42};

    EXPECT_CALL(btsPortMock, sendAttachRequest(btsId));
    EXPECT_CALL(timerPortMock, startTimer(std::chrono::milliseconds{500}));
    app->handleSib(btsId);

    EXPECT_CALL(timerPortMock, stopTimer());
    EXPECT_CALL(userPortMock, showConnected());
    app->handleAttachAccept();
}

TEST_F(ApplicationTestSuite, HandlesAttachRejectionCorrectly) {
    initApp();
    clearExpectations();

    common::BtsId btsId{42};
    EXPECT_CALL(btsPortMock, sendAttachRequest(btsId));
    EXPECT_CALL(timerPortMock, startTimer(std::chrono::milliseconds{500}));
    app->handleSib(btsId);

    EXPECT_CALL(timerPortMock, stopTimer());
    EXPECT_CALL(userPortMock, showNotConnected());
    app->handleAttachReject();
}

TEST_F(ApplicationTestSuite, HandlesAttachTimeoutProperly) {
    initApp();
    clearExpectations();

    common::BtsId btsId{42};
    EXPECT_CALL(btsPortMock, sendAttachRequest(btsId));
    EXPECT_CALL(timerPortMock, startTimer(std::chrono::milliseconds{500}));
    app->handleSib(btsId);

    EXPECT_CALL(timerPortMock, stopTimer());
    EXPECT_CALL(userPortMock, showNotConnected());
    app->handleTimeout();
}

TEST_F(ApplicationTestSuite, HandlesDisconnectionAfterAttach) {
    initApp();
    clearExpectations();

    common::BtsId btsId{42};
    EXPECT_CALL(btsPortMock, sendAttachRequest(btsId));
    EXPECT_CALL(timerPortMock, startTimer(std::chrono::milliseconds{500}));
    app->handleSib(btsId);

    EXPECT_CALL(timerPortMock, stopTimer());
    EXPECT_CALL(userPortMock, showConnected());
    app->handleAttachAccept();

    EXPECT_CALL(userPortMock, showNotConnected());
    app->handleDisconnected();
}

TEST_F(ApplicationTestSuite, ReceivesAndDisplaysSmsProperly) {
    initApp();
    clearExpectations();

    common::BtsId btsId{42};
    EXPECT_CALL(btsPortMock, sendAttachRequest(btsId));
    EXPECT_CALL(timerPortMock, startTimer(std::chrono::milliseconds{500}));
    app->handleSib(btsId);

    EXPECT_CALL(timerPortMock, stopTimer());
    EXPECT_CALL(userPortMock, showConnected());
    app->handleAttachAccept();

    Mock::VerifyAndClearExpectations(&userPortMock);

    EXPECT_CALL(userPortMock, showSms());
    app->handleSmsReceive(common::PhoneNumber{111}, "Hello from test");

    EXPECT_CALL(userPortMock, showNotConnected());
    app->handleDisconnected();
}

TEST_F(ApplicationTestSuite, shallHandleIncomingCallRequest)
{
    initApp();
    clearExpectations();
    common::PhoneNumber from{123};

    common::BtsId btsId{42};
    EXPECT_CALL(btsPortMock, sendAttachRequest(btsId));
    EXPECT_CALL(timerPortMock, startTimer(::testing::_));
    app->handleSib(btsId);

    EXPECT_CALL(timerPortMock, stopTimer()).Times(::testing::AnyNumber());
    EXPECT_CALL(userPortMock, showConnected());
    app->handleAttachAccept();

    clearExpectations();

    EXPECT_CALL(userPortMock, showIncomingCall(from));
    EXPECT_CALL(timerPortMock, startTimer(::testing::_)).Times(::testing::AnyNumber());
    app->handleCallRequest(from);

    EXPECT_CALL(btsPortMock, sendAcceptCall(from));
    EXPECT_CALL(userPortMock, showTalkingMobileScreen(from));
    app->handleAcceptCall(from);
}

TEST_F(ApplicationTestSuite, shallRejectIncomingCallRequest)
{
    initApp();
    clearExpectations();
    common::PhoneNumber from{123};
    common::BtsId btsId{42};

    EXPECT_CALL(btsPortMock, sendAttachRequest(btsId));
    EXPECT_CALL(timerPortMock, startTimer(::testing::_));
    app->handleSib(btsId);

    EXPECT_CALL(timerPortMock, stopTimer()).Times(::testing::AnyNumber());
    EXPECT_CALL(userPortMock, showConnected());
    app->handleAttachAccept();

    clearExpectations();

    EXPECT_CALL(userPortMock, showIncomingCall(from));
    EXPECT_CALL(timerPortMock, startTimer(::testing::_)).Times(::testing::AnyNumber());
    app->handleCallRequest(from);

    EXPECT_CALL(btsPortMock, sendRejectCall(from));
    EXPECT_CALL(userPortMock, showConnected());
    app->handleUiBack();
}

} // namespace ue
