#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "Ports/UserPort.hpp"
#include "Mocks/ILoggerMock.hpp"
#include "Mocks/IUserPortMock.hpp"
#include "Mocks/IUeGuiMock.hpp"
#include "Messages/PhoneNumber.hpp"
#include <functional> // Include for std::function matcher

namespace ue
{
using namespace ::testing;

using LoggerNiceMock = NiceMock<common::ILoggerMock>;
using GuiStrictMock = StrictMock<IUeGuiMock>;
using HandlerStrictMock = StrictMock<IUserEventsHandlerMock>;
using ListViewModeStrictMock = StrictMock<IListViewModeMock>;
using TextModeStrictMock = StrictMock<ITextModeMock>;
using SmsComposeModeStrictMock = StrictMock<ISmsComposeModeMock>;


class UserPortTestSuite : public Test
{
protected:
    const common::PhoneNumber PHONE_NUMBER{112};
    LoggerNiceMock loggerMock;
    GuiStrictMock guiMock;
    HandlerStrictMock handlerMock;
    ListViewModeStrictMock listViewModeMock;
    TextModeStrictMock textModeMock;
    SmsComposeModeStrictMock smsComposeModeMock;

    UserPort objectUnderTest{loggerMock, guiMock, PHONE_NUMBER};

    void SetUp() override {
         // Common expectations for start()
         EXPECT_CALL(guiMock, setTitle(HasSubstr(common::to_string(PHONE_NUMBER))));
         EXPECT_CALL(guiMock, setRejectCallback(_)); // Use _ as specific function object isn't easily comparable
         EXPECT_CALL(guiMock, setAcceptCallback(Eq(nullptr))); // Expect null initially
         objectUnderTest.start(handlerMock);
         Mock::VerifyAndClearExpectations(&guiMock); // Clear setup expectations
    }

    void TearDown() override {
         objectUnderTest.stop();
    }

};

TEST_F(UserPortTestSuite, shallStartStop)
{
    // Re-apply start expectations if needed after ClearExpectations in SetUp
    // or if TearDown affected state. Assuming TearDown doesn't clear mocks here.
     EXPECT_CALL(guiMock, setTitle(HasSubstr(common::to_string(PHONE_NUMBER))));
     EXPECT_CALL(guiMock, setRejectCallback(_));
     EXPECT_CALL(guiMock, setAcceptCallback(Eq(nullptr)));
     objectUnderTest.start(handlerMock);
}

TEST_F(UserPortTestSuite, shallShowNotConnected)
{
    EXPECT_CALL(guiMock, setAcceptCallback(Eq(nullptr)));
    EXPECT_CALL(guiMock, showNotConnected());
    objectUnderTest.showNotConnected();
}

TEST_F(UserPortTestSuite, shallShowConnecting)
{
    EXPECT_CALL(guiMock, setAcceptCallback(Eq(nullptr)));
    EXPECT_CALL(guiMock, showConnecting());
    objectUnderTest.showConnecting();
}

TEST_F(UserPortTestSuite, shallShowMenuOnConnected)
{
    // Define the sequence of expected calls within showConnected
    Sequence seq;

    // CORRECTED: Reordered expectations to match code execution
    EXPECT_CALL(guiMock, setListViewMode())                     .InSequence(seq).WillOnce(ReturnRef(listViewModeMock));
    EXPECT_CALL(listViewModeMock, clearSelectionList())         .InSequence(seq);
    EXPECT_CALL(guiMock, setTitle(HasSubstr(common::to_string(PHONE_NUMBER)))).InSequence(seq); // Title reset happens after list setup
    EXPECT_CALL(listViewModeMock, addSelectionListItem("Compose SMS", "sms.compose")).InSequence(seq);
    EXPECT_CALL(listViewModeMock, addSelectionListItem("View SMS", "sms.view")).InSequence(seq);
    EXPECT_CALL(listViewModeMock, addSelectionListItem("Dial Call", "call.dial")).InSequence(seq);
    EXPECT_CALL(guiMock, setAcceptCallback(_))                  .InSequence(seq); // Callback set for list view
    EXPECT_CALL(guiMock, showNewSms(false))                     .InSequence(seq); // Indicator cleared at the end

    // Trigger the method under test
    objectUnderTest.showConnected();
}

}