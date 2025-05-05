#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "Application.hpp"
#include "Context.hpp"
#include "Messages/PhoneNumber.hpp"

#include "States/ReceivingCallState.hpp"
#include "States/DialingState.hpp"
#include "States/TalkingState.hpp"
#include "States/ConnectedState.hpp"

#include "Mocks/ILoggerMock.hpp"
#include "Mocks/IUserPortMock.hpp"
#include "Mocks/IBtsPortMock.hpp"
#include "Mocks/ITimerPortMock.hpp"
#include "Mocks/ICallModeMock.hpp"

using namespace ue;
using namespace common;
using ::testing::StrictMock;
using ::testing::NiceMock;
using ::testing::InSequence;
using ::testing::_;
using ::testing::ReturnRef;
using ::testing::Return;

struct CallStateTest : testing::Test {
    using LoggerMock = NiceMock<common::ILoggerMock>;
    LoggerMock                   loggerMock;
    NiceMock<IUserPortMock>      user;
    StrictMock<IBtsPortMock>     bts;
    NiceMock<ITimerPortMock>   timer;
    NiceMock<ICallModeMock>      callMode;
    Application                  app{PhoneNumber{111}, loggerMock, bts, user, timer};
    Context&                     ctx = app.getContext();
    const PhoneNumber            peer{222};

    CallStateTest() {
        ON_CALL(user, showCallMode())
            .WillByDefault(ReturnRef(callMode));

        ON_CALL(user, showTalkingOverlay())
            .WillByDefault(testing::InvokeWithoutArgs([](){}));
    }
};

TEST_F(CallStateTest, ReceivingCall_AcceptToTalking) {
    InSequence s;

    // Incoming call
    EXPECT_CALL(user, showIncomingCall(peer));
    EXPECT_CALL(timer, startTimer(_));
    ctx.setState<ReceivingCallState>(peer);

    // Accept button pressed
    EXPECT_CALL(timer, stopTimer());
    EXPECT_CALL(bts, sendCallAccepted(peer));

    EXPECT_CALL(user, showTalkingOverlay());
    EXPECT_CALL(user, showCallMode()).WillOnce(ReturnRef(callMode));

    EXPECT_CALL(callMode, clearIncomingText());
    EXPECT_CALL(callMode, clearOutgoingText());
    EXPECT_CALL(bts, sendCallAccepted(peer));
    EXPECT_CALL(callMode, appendIncomingText("Talking"));

    ctx.state->handleUserAction("ACCEPT");
}

TEST_F(CallStateTest, Dialing_AcceptedByPeerToTalking) {
    InSequence s;

    // Outcoming call
    EXPECT_CALL(user, showDialing());
    EXPECT_CALL(timer, startTimer(_));
    ctx.setState<DialingState>();

    // Number input and accept (call) button pressed
    EXPECT_CALL(user, getDialedNumber(_))
        .WillOnce(testing::DoAll(
            testing::SetArgReferee<0>(peer),
            testing::Return(true)
            ));
    EXPECT_CALL(bts, sendCallRequest(peer));
    ctx.state->handleUserAction("ACCEPT");

    // BTS confirms accept
    EXPECT_CALL(timer, stopTimer());
    EXPECT_CALL(user, showTalkingOverlay());
    EXPECT_CALL(user, showCallMode()).WillOnce(ReturnRef(callMode));
    EXPECT_CALL(callMode, clearIncomingText());
    EXPECT_CALL(callMode, clearOutgoingText());
    EXPECT_CALL(bts, sendCallAccepted(peer));
    EXPECT_CALL(callMode, appendIncomingText("Talking"));

    ctx.state->handleCallAccepted(peer);
}

TEST_F(CallStateTest, Talking_RedButtonDropsAndReturns) {
    InSequence s;

    ctx.setState<TalkingState>(peer);
    // Reject button pressed
    EXPECT_CALL(bts, sendCallDropped(peer));
    ctx.state->handleUserAction("REJECT");
    // return to ConnectedState
    EXPECT_NE(nullptr, dynamic_cast<ConnectedState*>(ctx.state.get()));
}
