#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "Ports/UserPort.hpp"
#include "Mocks/ILoggerMock.hpp"
#include "Mocks/IUserPortMock.hpp"
#include "Messages/PhoneNumber.hpp"
#include "Mocks/IUeGuiMock.hpp"

namespace ue
{
using namespace ::testing;

class UserPortTestSuite : public Test
{
protected:
    const common::PhoneNumber PHONE_NUMBER{112};
    NiceMock<common::ILoggerMock> loggerMock;
    StrictMock<IUserEventsHandlerMock> handlerMock;
    StrictMock<IUeGuiMock> guiMock;
    StrictMock<IListViewModeMock> listViewModeMock;
    StrictMock<ITextModeMock> textModeMock;
    StrictMock<ICallModeMock> callModeMock;
    StrictMock<ISmsComposeModeMock> composeModeMock;
    UserPort objectUnderTest{loggerMock, guiMock, PHONE_NUMBER};

    UserPortTestSuite()
    {
        EXPECT_CALL(guiMock, setTitle(HasSubstr(to_string(PHONE_NUMBER))));
        objectUnderTest.start(handlerMock);
    }
    ~UserPortTestSuite()
    {
        objectUnderTest.stop();
    }
};

TEST_F(UserPortTestSuite, shallStartStop)
{
}

TEST_F(UserPortTestSuite, shallShowNotConnected)
{
    EXPECT_CALL(guiMock, showNotConnected());
    objectUnderTest.showNotConnected();
}

TEST_F(UserPortTestSuite, shallShowConnecting)
{
    EXPECT_CALL(guiMock, showConnecting());
    objectUnderTest.showConnecting();
}

TEST_F(UserPortTestSuite, shallShowMenuOnConnected)
{
    EXPECT_CALL(guiMock, setListViewMode()).WillOnce(ReturnRef(listViewModeMock));
    EXPECT_CALL(listViewModeMock, clearSelectionList());
    EXPECT_CALL(listViewModeMock, addSelectionListItem(_, _)).Times(AtLeast(1));
    EXPECT_CALL(guiMock, setAcceptCallback(_)).Times(Exactly(1));
    EXPECT_CALL(guiMock, setRejectCallback(_)).Times(Exactly(1));

    objectUnderTest.showConnected();
}

TEST_F(UserPortTestSuite, shallShowSmsList)
{
    std::vector<SmsEntity> smsVector;
    smsVector.push_back({12,123,"test1"});
    smsVector.push_back({12,123,"test2"});
    smsVector.push_back({123,12,"test3"});

    EXPECT_CALL(guiMock, setListViewMode()).WillOnce(ReturnRef(listViewModeMock));
    EXPECT_CALL(listViewModeMock, clearSelectionList());
    EXPECT_CALL(listViewModeMock, addSelectionListItem(_, _)).Times(Exactly(3));
    EXPECT_CALL(guiMock, showNewSms(false)).Times(Exactly(1));
    EXPECT_CALL(guiMock, setAcceptCallback(_)).Times(Exactly(1));
    EXPECT_CALL(guiMock, setRejectCallback(_)).Times(Exactly(1));

    objectUnderTest.showSmsList(smsVector);
}

TEST_F(UserPortTestSuite, shallShowSms)
{
    SmsEntity testSms{12,123,"test"};

    EXPECT_CALL(guiMock, setViewTextMode()).WillOnce(ReturnRef(textModeMock));
    EXPECT_CALL(textModeMock, setText("test"));
    EXPECT_CALL(guiMock, setAcceptCallback(_)).Times(Exactly(1));
    EXPECT_CALL(guiMock, setRejectCallback(_)).Times(Exactly(1));

    objectUnderTest.showSms(testSms);
}

TEST_F(UserPortTestSuite, showTalking_setsUpGuiAndAcceptSendsText)
{
    EXPECT_CALL(guiMock, setCallMode()).WillOnce(ReturnRef(callModeMock));
    EXPECT_CALL(callModeMock, clearIncomingText());
    EXPECT_CALL(guiMock, setAcceptCallback(_));
    EXPECT_CALL(guiMock, setRejectCallback(_));

    objectUnderTest.showTalking(common::PhoneNumber{123});
}

TEST_F(UserPortTestSuite, displayMessage_appendsPeerText)
{
    EXPECT_CALL(guiMock, setCallMode()).WillOnce(ReturnRef(callModeMock));
    EXPECT_CALL(callModeMock, appendIncomingText("Peer: Hello"));

    objectUnderTest.displayMessage(common::PhoneNumber{123}, "Hello");
}

TEST_F(UserPortTestSuite, shallComposeSmsAndSend)
{
    SmsEntity sms{PHONE_NUMBER.value, 123, "Hello!", false};

    EXPECT_CALL(guiMock, setSmsComposeMode()).WillOnce(ReturnRef(composeModeMock));
    EXPECT_CALL(composeModeMock, clearSmsText());
    EXPECT_CALL(composeModeMock, getPhoneNumber()).WillOnce(Return(common::PhoneNumber{123}));
    EXPECT_CALL(composeModeMock, getSmsText()).WillOnce(Return("Hello!"));
    EXPECT_CALL(handlerMock, sendSms(sms));

    IUeGui::Callback acceptCallback;
    EXPECT_CALL(guiMock, setAcceptCallback(_)).WillOnce(SaveArg<0>(&acceptCallback));
    EXPECT_CALL(guiMock, setRejectCallback(_));

    objectUnderTest.composeSms();
    acceptCallback();
}

TEST_F(UserPortTestSuite, shallStartDialAndSendCallRequest)
{
    StrictMock<IDialModeMock> dialModeMock;

    EXPECT_CALL(guiMock, setDialMode()).WillOnce(ReturnRef(dialModeMock));
    EXPECT_CALL(dialModeMock, getPhoneNumber()).WillOnce(Return(common::PhoneNumber{123}));
    EXPECT_CALL(handlerMock, sendCallRequest(common::PhoneNumber{123}));

    EXPECT_CALL(guiMock, setAcceptCallback(_)).WillOnce(Invoke([](auto cb){ cb(); }));
    EXPECT_CALL(guiMock, setRejectCallback(_));

    objectUnderTest.startDial();
}

TEST_F(UserPortTestSuite, shallShowNewSms)
{
    EXPECT_CALL(guiMock, showNewSms(true));
    objectUnderTest.showNewSms();
}

TEST_F(UserPortTestSuite, shallReturnPhoneNumber)
{
    EXPECT_EQ(objectUnderTest.getPhoneNumber(), PHONE_NUMBER);
}

TEST_F(UserPortTestSuite, shallStartDial)
{
    StrictMock<IDialModeMock> dialModeMock;
    EXPECT_CALL(guiMock, setDialMode()).WillOnce(ReturnRef(dialModeMock));
    EXPECT_CALL(guiMock, setAcceptCallback(_));
    EXPECT_CALL(guiMock, setRejectCallback(_));
    objectUnderTest.startDial();
}

TEST_F(UserPortTestSuite, shallShowDialingAndCancel)
{
    StrictMock<ITextModeMock> alertModeMock;
    EXPECT_CALL(guiMock, setAlertMode()).WillOnce(ReturnRef(alertModeMock));
    EXPECT_CALL(alertModeMock, setText("Dialing..."));
    EXPECT_CALL(guiMock, setRejectCallback(_)).WillOnce(Invoke([this](auto cb) {
        cb();
    }));
    EXPECT_CALL(handlerMock, cancelCallRequest());
    objectUnderTest.showDialing();
}

TEST_F(UserPortTestSuite, showTalking_acceptSendsTextWhenNotEmpty)
{
    StrictMock<ICallModeMock> callModeMock;

    EXPECT_CALL(guiMock, setCallMode()).WillOnce(ReturnRef(callModeMock));
    EXPECT_CALL(callModeMock, clearIncomingText());
    EXPECT_CALL(guiMock, setAcceptCallback(_)).WillOnce(Invoke([&](auto cb) {
        EXPECT_CALL(callModeMock, getOutgoingText()).WillOnce(Return("Hello"));
        EXPECT_CALL(callModeMock, appendIncomingText("You: Hello"));
        EXPECT_CALL(handlerMock, sendTalkMessage("Hello"));
        EXPECT_CALL(callModeMock, clearOutgoingText());
        cb();
    }));
    EXPECT_CALL(guiMock, setRejectCallback(_));

    objectUnderTest.showTalking(common::PhoneNumber{123});
}

TEST_F(UserPortTestSuite, shallShowPartnerNotAvailable)
{
    common::PhoneNumber TO{113};
    EXPECT_CALL(guiMock, showPeerUserNotAvailable(TO));
    EXPECT_CALL(guiMock, setRejectCallback(_));
    EXPECT_CALL(guiMock, setAcceptCallback(_));
    objectUnderTest.showPartnerNotAvailable(TO);
}

}
