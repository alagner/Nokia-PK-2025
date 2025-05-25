#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "Application.hpp"
#include "Mocks/ILoggerMock.hpp"
#include "Mocks/IBtsPortMock.hpp"
#include "Mocks/IUserPortMock.hpp"
#include "Mocks/ITimerPortMock.hpp"
#include "Mocks/ISmsDbMock.hpp"
#include "Messages/PhoneNumber.hpp"
#include "Messages/BtsId.hpp"
#include <memory>

namespace ue
{
using namespace ::testing;

class ApplicationTestSuite : public Test
{
protected:
    const common::PhoneNumber PHONE_NUMBER{42};
    NiceMock<common::ILoggerMock> loggerMock;
    NiceMock<IBtsPortMock> btsPortMock;
    NiceMock<IUserPortMock> userPortMock;
    NiceMock<ITimerPortMock> timerPortMock;
    NiceMock<ISmsDbMock> smsDbMock;

    Application objectUnderTest;

    ApplicationTestSuite()
        : objectUnderTest(PHONE_NUMBER,
                         loggerMock,
                         btsPortMock,
                         userPortMock,
                         timerPortMock,
                         smsDbMock)
    {}
};

struct ViewSmsTestSuite : ApplicationTestSuite
{
protected:
    void SetUp() override
    {
        EXPECT_CALL(userPortMock, showConnecting());
        EXPECT_CALL(btsPortMock, sendAttachRequest(common::BtsId{1}));
        objectUnderTest.handleSib(common::BtsId{1});

        EXPECT_CALL(userPortMock, showConnected());
        objectUnderTest.handleAttachAccept();
    }

    std::vector<Sms> emptySmsVector;  // Empty vector for tests
};

TEST_F(ViewSmsTestSuite, ShouldDisplaySmsListWhenViewingSms)
{
    // Allow for any showNewSms calls that might happen
    EXPECT_CALL(userPortMock, showNewSms(_))
        .Times(AnyNumber());

    EXPECT_CALL(smsDbMock, getAllSms())
        .Times(AtLeast(1))
        .WillRepeatedly(ReturnRef(emptySmsVector));
    EXPECT_CALL(smsDbMock, hasUnreadSms())
        .Times(AtLeast(1))
        .WillRepeatedly(Return(false));

    EXPECT_CALL(userPortMock, setSmsList(Ref(emptySmsVector)))
        .Times(AtLeast(1));
    EXPECT_CALL(userPortMock, setSelectSmsCallback(_))
        .Times(AtLeast(1));
    EXPECT_CALL(userPortMock, showSmsList())
        .Times(AtLeast(1));

    objectUnderTest.viewSms();

    objectUnderTest.selectSms(0);

    EXPECT_CALL(userPortMock, showConnected());
    objectUnderTest.closeSmsView();
}

TEST_F(ViewSmsTestSuite, ShouldDisplaySmsContentWhenSelectingSms)
{
    const common::PhoneNumber senderNumber{123};
    const std::string smsText = "Test message";

    // Set up mock to expect SMS addition
    EXPECT_CALL(smsDbMock, addSms(senderNumber, smsText));
    EXPECT_CALL(userPortMock, showNewSms(_))
        .Times(AnyNumber());

    objectUnderTest.handleSms(senderNumber, smsText);

    // Prepare mock data for viewing SMS
    std::vector<Sms> smsDbWithOneMessage = {
        Sms{senderNumber, smsText}
    };

    EXPECT_CALL(smsDbMock, getAllSms())
        .Times(AtLeast(1))
        .WillRepeatedly(ReturnRef(smsDbWithOneMessage));
    EXPECT_CALL(smsDbMock, hasUnreadSms())
        .Times(AtLeast(1))
        .WillRepeatedly(Return(false));

    EXPECT_CALL(userPortMock, setSmsList(Ref(smsDbWithOneMessage)))
        .Times(AtLeast(1));
    EXPECT_CALL(userPortMock, setSelectSmsCallback(_))
        .Times(AtLeast(1));
    EXPECT_CALL(userPortMock, showSmsList())
        .Times(AtLeast(1));

    objectUnderTest.viewSms();

    EXPECT_CALL(userPortMock, showSmsContent(std::to_string(senderNumber.value), smsText));
    objectUnderTest.selectSms(0);

    EXPECT_CALL(userPortMock, showConnected());
    objectUnderTest.closeSmsView();
}

TEST_F(ViewSmsTestSuite, ShouldMarkSmsAsReadWhenViewing)
{
    const common::PhoneNumber senderNumber{123};
    const std::string smsText = "Test message";

    // Set up mock to expect SMS addition
    EXPECT_CALL(smsDbMock, addSms(senderNumber, smsText));
    EXPECT_CALL(userPortMock, showNewSms(_))
        .Times(AnyNumber());
    objectUnderTest.handleSms(senderNumber, smsText);

    std::vector<Sms> smsDbWithOneUnreadMessage = {
        Sms{senderNumber, smsText, false}
    };

    EXPECT_CALL(smsDbMock, getAllSms())
        .Times(AtLeast(1))
        .WillRepeatedly(ReturnRef(smsDbWithOneUnreadMessage));
    EXPECT_CALL(smsDbMock, hasUnreadSms())
        .Times(AtLeast(1))
        .WillRepeatedly(Return(false));

    EXPECT_CALL(userPortMock, setSmsList(Ref(smsDbWithOneUnreadMessage)))
        .Times(AtLeast(1));
    EXPECT_CALL(userPortMock, setSelectSmsCallback(_))
        .Times(AtLeast(1));
    EXPECT_CALL(userPortMock, showSmsList())
        .Times(AtLeast(1));

    objectUnderTest.viewSms();

    // When selecting SMS, expect it to be marked as read
    EXPECT_CALL(smsDbMock, markAsRead(0));
    EXPECT_CALL(userPortMock, showSmsContent(std::to_string(senderNumber.value), smsText));
    objectUnderTest.selectSms(0);
}

TEST_F(ViewSmsTestSuite, ShouldAllowViewingMultipleSmsInSequence)
{
    const common::PhoneNumber sender1{43};
    const common::PhoneNumber sender2{44};
    const std::string text1 = "Hello1";
    const std::string text2 = "Hello2";

    // Add two SMS messages
    EXPECT_CALL(smsDbMock, addSms(sender1, text1));
    EXPECT_CALL(userPortMock, showNewSms(_))
        .Times(AnyNumber());
    objectUnderTest.handleSms(sender1, text1);

    EXPECT_CALL(smsDbMock, addSms(sender2, text2));
    objectUnderTest.handleSms(sender2, text2);

    std::vector<Sms> smsDbWithTwoMessages = {
        Sms{sender1, text1},
        Sms{sender2, text2}
    };

    // First viewing
    EXPECT_CALL(smsDbMock, getAllSms())
        .Times(AtLeast(1))
        .WillRepeatedly(ReturnRef(smsDbWithTwoMessages));
    EXPECT_CALL(smsDbMock, hasUnreadSms())
        .Times(AtLeast(1))
        .WillRepeatedly(Return(false));

    EXPECT_CALL(userPortMock, setSmsList(Ref(smsDbWithTwoMessages)))
        .Times(AtLeast(1));
    EXPECT_CALL(userPortMock, setSelectSmsCallback(_))
        .Times(AtLeast(1));
    EXPECT_CALL(userPortMock, showSmsList())
        .Times(AtLeast(1));

    objectUnderTest.viewSms();

    // View first message
    EXPECT_CALL(userPortMock, showSmsContent(std::to_string(sender1.value), text1));
    objectUnderTest.selectSms(0);

    EXPECT_CALL(userPortMock, showConnected());
    objectUnderTest.closeSmsView();  // Return to main menu

    // Second viewing - reset expectations for the second viewing
    EXPECT_CALL(smsDbMock, getAllSms())
        .Times(AtLeast(1))
        .WillRepeatedly(ReturnRef(smsDbWithTwoMessages));
    EXPECT_CALL(smsDbMock, hasUnreadSms())
        .Times(AtLeast(1))
        .WillRepeatedly(Return(false));

    EXPECT_CALL(userPortMock, setSmsList(Ref(smsDbWithTwoMessages)))
        .Times(AtLeast(1));
    EXPECT_CALL(userPortMock, setSelectSmsCallback(_))
        .Times(AtLeast(1));
    EXPECT_CALL(userPortMock, showSmsList())
        .Times(AtLeast(1));

    objectUnderTest.viewSms();

    // View second message
    EXPECT_CALL(userPortMock, showSmsContent(std::to_string(sender2.value), text2));
    objectUnderTest.selectSms(1);

    // Return to main menu
    EXPECT_CALL(userPortMock, showConnected());
    objectUnderTest.closeSmsView();
}

// Tests for interactions with Viewing SMS
TEST_F(ViewSmsTestSuite, ShallCloseImmediatelyWhenUserClosesWhileViewingSms)
{
    const common::PhoneNumber sender{123};
    const std::string text = "Test message";

    EXPECT_CALL(smsDbMock, addSms(sender, text));
    EXPECT_CALL(userPortMock, showNewSms(_))
        .Times(AnyNumber());
    objectUnderTest.handleSms(sender, text);

    std::vector<Sms> smsDbWithMessage = {
        Sms{sender, text}
    };

    EXPECT_CALL(smsDbMock, getAllSms())
        .Times(AtLeast(1))
        .WillRepeatedly(ReturnRef(smsDbWithMessage));
    EXPECT_CALL(smsDbMock, hasUnreadSms())
        .Times(AtLeast(1))
        .WillRepeatedly(Return(false));

    EXPECT_CALL(userPortMock, setSmsList(Ref(smsDbWithMessage)))
        .Times(AtLeast(1));
    EXPECT_CALL(userPortMock, setSelectSmsCallback(_))
        .Times(AtLeast(1));
    EXPECT_CALL(userPortMock, showSmsList())
        .Times(AtLeast(1));

    objectUnderTest.viewSms();

    testing::Mock::VerifyAndClearExpectations(&userPortMock);
    testing::Mock::VerifyAndClearExpectations(&smsDbMock);

    objectUnderTest.handleClose();

}

TEST_F(ViewSmsTestSuite, ShallGoToNotConnectedStateImmediatelyWhenBtsConnectionDroppedWhileViewingSms)
{
    const common::PhoneNumber sender{123};
    const std::string text = "Test message";
    
    EXPECT_CALL(smsDbMock, addSms(sender, text));
    EXPECT_CALL(userPortMock, showNewSms(_))
        .Times(AnyNumber());
    objectUnderTest.handleSms(sender, text);

    std::vector<Sms> smsDbWithMessage = {
        Sms{sender, text}
    };

    EXPECT_CALL(smsDbMock, getAllSms())
        .Times(AtLeast(1))
        .WillRepeatedly(ReturnRef(smsDbWithMessage));
    EXPECT_CALL(smsDbMock, hasUnreadSms())
        .Times(AtLeast(1))
        .WillRepeatedly(Return(false));
    
    EXPECT_CALL(userPortMock, setSmsList(Ref(smsDbWithMessage)))
        .Times(AtLeast(1));
    EXPECT_CALL(userPortMock, setSelectSmsCallback(_))
        .Times(AtLeast(1));
    EXPECT_CALL(userPortMock, showSmsList())
        .Times(AtLeast(1));
        
    objectUnderTest.viewSms();
    
    testing::Mock::VerifyAndClearExpectations(&userPortMock);
    testing::Mock::VerifyAndClearExpectations(&smsDbMock);

    EXPECT_CALL(userPortMock, showNotConnected());
    objectUnderTest.handleDisconnect();
    
    EXPECT_CALL(userPortMock, showConnecting());
    EXPECT_CALL(btsPortMock, sendAttachRequest(common::BtsId{2}));
    objectUnderTest.handleSib(common::BtsId{2});
    
}

TEST_F(ViewSmsTestSuite, ShallStoreAndNotInterruptViewingWhenReceivingSmsWhileViewingSms)
{
    const common::PhoneNumber sender1{43};
    const std::string text1 = "Initial message";

    EXPECT_CALL(smsDbMock, addSms(sender1, text1));
    EXPECT_CALL(userPortMock, showNewSms(_))
        .Times(AnyNumber());
    objectUnderTest.handleSms(sender1, text1);

    std::vector<Sms> smsDbWithOneMessage = {
        Sms{sender1, text1}
    };

    EXPECT_CALL(smsDbMock, getAllSms())
        .Times(AtLeast(1))
        .WillRepeatedly(ReturnRef(smsDbWithOneMessage));
    EXPECT_CALL(smsDbMock, hasUnreadSms())
        .Times(AtLeast(1))
        .WillRepeatedly(Return(false));
    
    EXPECT_CALL(userPortMock, setSmsList(Ref(smsDbWithOneMessage)))
        .Times(AtLeast(1));
    EXPECT_CALL(userPortMock, setSelectSmsCallback(_))
        .Times(AtLeast(1));
    EXPECT_CALL(userPortMock, showSmsList())
        .Times(AtLeast(1));
        
    objectUnderTest.viewSms();

    EXPECT_CALL(userPortMock, showSmsContent(std::to_string(sender1.value), text1));
    objectUnderTest.selectSms(0);

    testing::Mock::VerifyAndClearExpectations(&userPortMock);
    testing::Mock::VerifyAndClearExpectations(&smsDbMock);

    const common::PhoneNumber sender2{44};
    const std::string text2 = "New message while viewing";

    EXPECT_CALL(smsDbMock, addSms(sender2, text2));
    EXPECT_CALL(userPortMock, showNewSms(_))
        .Times(AnyNumber());
        
    objectUnderTest.handleSms(sender2, text2);

    EXPECT_CALL(userPortMock, showConnected());
    objectUnderTest.closeSmsView();

    std::vector<Sms> updatedSmsDb = {
        Sms{sender1, text1},
        Sms{sender2, text2}
    };

    EXPECT_CALL(smsDbMock, getAllSms())
        .Times(AtLeast(1))
        .WillRepeatedly(ReturnRef(updatedSmsDb));
    EXPECT_CALL(smsDbMock, hasUnreadSms())
        .Times(AnyNumber())
        .WillRepeatedly(Return(true));
    
    EXPECT_CALL(userPortMock, setSmsList(Ref(updatedSmsDb)))
        .Times(AtLeast(1));
    EXPECT_CALL(userPortMock, setSelectSmsCallback(_))
        .Times(AtLeast(1));
    EXPECT_CALL(userPortMock, showSmsList())
        .Times(AtLeast(1));
        
    objectUnderTest.viewSms();

    EXPECT_CALL(userPortMock, showSmsContent(std::to_string(sender2.value), text2));
    objectUnderTest.selectSms(1);
}

TEST_F(ViewSmsTestSuite, ShallInterruptViewingSmsWhenReceivingCallRequest)
{
    const common::PhoneNumber sender{123};
    const std::string text = "Test message";
    
    EXPECT_CALL(smsDbMock, addSms(sender, text));
    EXPECT_CALL(userPortMock, showNewSms(_))
        .Times(AnyNumber());
    objectUnderTest.handleSms(sender, text);

    std::vector<Sms> smsDbWithMessage = {
        Sms{sender, text}
    };


    EXPECT_CALL(smsDbMock, getAllSms())
        .Times(AtLeast(1))
        .WillRepeatedly(ReturnRef(smsDbWithMessage));
    EXPECT_CALL(smsDbMock, hasUnreadSms())
        .Times(AtLeast(1))
        .WillRepeatedly(Return(false));
    
    EXPECT_CALL(userPortMock, setSmsList(Ref(smsDbWithMessage)))
        .Times(AtLeast(1));
    EXPECT_CALL(userPortMock, setSelectSmsCallback(_))
        .Times(AtLeast(1));
    EXPECT_CALL(userPortMock, showSmsList())
        .Times(AtLeast(1));
        
    objectUnderTest.viewSms();
    
    testing::Mock::VerifyAndClearExpectations(&userPortMock);
    testing::Mock::VerifyAndClearExpectations(&smsDbMock);
    testing::Mock::VerifyAndClearExpectations(&timerPortMock);

    const common::PhoneNumber callerNumber{156};

    EXPECT_CALL(timerPortMock, startTimer(_));

    EXPECT_CALL(userPortMock, showCallRequest(callerNumber));

    EXPECT_CALL(userPortMock, showSmsList())
        .Times(0);
    EXPECT_CALL(userPortMock, showSmsContent(_, _))
        .Times(0);

    objectUnderTest.handleCallRequest(callerNumber);

    testing::Mock::VerifyAndClearExpectations(&userPortMock);
    testing::Mock::VerifyAndClearExpectations(&timerPortMock);
    testing::Mock::VerifyAndClearExpectations(&btsPortMock);

    EXPECT_CALL(timerPortMock, stopTimer());
    EXPECT_CALL(btsPortMock, sendCallAccept(callerNumber));

    EXPECT_CALL(userPortMock, showSmsList())
        .Times(0);
    EXPECT_CALL(userPortMock, showSmsContent(_, _))
        .Times(0);
    
    objectUnderTest.acceptCallRequest();

    testing::Mock::VerifyAndClearExpectations(&userPortMock);
    testing::Mock::VerifyAndClearExpectations(&timerPortMock);
    testing::Mock::VerifyAndClearExpectations(&btsPortMock);

    EXPECT_CALL(userPortMock, showConnected());
    objectUnderTest.handleCallDropped(callerNumber);

    const common::PhoneNumber callerNumber2{189};
    
    EXPECT_CALL(timerPortMock, startTimer(_));
    EXPECT_CALL(userPortMock, showCallRequest(callerNumber2));
    objectUnderTest.handleCallRequest(callerNumber2);

    testing::Mock::VerifyAndClearExpectations(&userPortMock);
    testing::Mock::VerifyAndClearExpectations(&timerPortMock);
    testing::Mock::VerifyAndClearExpectations(&btsPortMock);
    
    EXPECT_CALL(timerPortMock, stopTimer());
    EXPECT_CALL(btsPortMock, sendCallDropped(callerNumber2));
    EXPECT_CALL(userPortMock, showConnected());
 
    EXPECT_CALL(userPortMock, showSmsList())
        .Times(0);
    EXPECT_CALL(userPortMock, showSmsContent(_, _))
        .Times(0);
    
    objectUnderTest.rejectCallRequest();
}

// Tests for interactions with Sending SMS
TEST_F(ViewSmsTestSuite, ShallCloseUeImmediatelyWhenClosingWhileSendingSms)
{
    testing::Mock::VerifyAndClearExpectations(&userPortMock);
    testing::Mock::VerifyAndClearExpectations(&btsPortMock);
    testing::Mock::VerifyAndClearExpectations(&timerPortMock);

    EXPECT_CALL(userPortMock, showSmsComposeView());
    objectUnderTest.composeSms();

    testing::Mock::VerifyAndClearExpectations(&userPortMock);
    testing::Mock::VerifyAndClearExpectations(&btsPortMock);
    testing::Mock::VerifyAndClearExpectations(&timerPortMock);

    objectUnderTest.handleClose();
}

TEST_F(ViewSmsTestSuite, ShallGoToNotConnectedStateImmediatelyWhenBtsConnectionDroppedWhileSendingSms)
{
    testing::Mock::VerifyAndClearExpectations(&userPortMock);
    testing::Mock::VerifyAndClearExpectations(&btsPortMock);
    testing::Mock::VerifyAndClearExpectations(&timerPortMock);

    EXPECT_CALL(userPortMock, showSmsComposeView());
    objectUnderTest.composeSms();

    testing::Mock::VerifyAndClearExpectations(&userPortMock);
    testing::Mock::VerifyAndClearExpectations(&btsPortMock);
    testing::Mock::VerifyAndClearExpectations(&timerPortMock);

    EXPECT_CALL(userPortMock, showNotConnected());
    objectUnderTest.handleDisconnect();

    EXPECT_CALL(userPortMock, showConnecting());
    EXPECT_CALL(btsPortMock, sendAttachRequest(common::BtsId{2}));
    objectUnderTest.handleSib(common::BtsId{2});
}

TEST_F(ViewSmsTestSuite, ShallStoreAndNotInterruptSendingWhenReceivingSmsWhileSendingSms)
{
    testing::Mock::VerifyAndClearExpectations(&userPortMock);
    testing::Mock::VerifyAndClearExpectations(&btsPortMock);
    testing::Mock::VerifyAndClearExpectations(&timerPortMock);
    testing::Mock::VerifyAndClearExpectations(&smsDbMock);

    EXPECT_CALL(userPortMock, showSmsComposeView());
    objectUnderTest.composeSms();

    testing::Mock::VerifyAndClearExpectations(&userPortMock);
    testing::Mock::VerifyAndClearExpectations(&btsPortMock);
    testing::Mock::VerifyAndClearExpectations(&smsDbMock);

    const common::PhoneNumber senderNumber{145};
    const std::string incomingText = "Incoming message while composing SMS";

    EXPECT_CALL(smsDbMock, addSms(senderNumber, incomingText));

    EXPECT_CALL(userPortMock, showNewSms(_))
        .Times(AnyNumber());

    objectUnderTest.handleSms(senderNumber, incomingText);

    testing::Mock::VerifyAndClearExpectations(&userPortMock);
    testing::Mock::VerifyAndClearExpectations(&smsDbMock);
    testing::Mock::VerifyAndClearExpectations(&btsPortMock);

    const common::PhoneNumber recipientNumber{156};
    const std::string outgoingText = "Outgoing reply message";

    EXPECT_CALL(btsPortMock, sendSms(recipientNumber, outgoingText));
    EXPECT_CALL(userPortMock, showConnected());

    objectUnderTest.acceptSmsCompose(recipientNumber, outgoingText);

    std::vector<Sms> expectedSmsDb = {
        Sms{senderNumber, incomingText}
    };
    
    EXPECT_CALL(smsDbMock, getAllSms())
        .Times(AtLeast(1))
        .WillRepeatedly(ReturnRef(expectedSmsDb));
    EXPECT_CALL(smsDbMock, hasUnreadSms())
        .Times(AtLeast(1))
        .WillRepeatedly(Return(true));
    
    EXPECT_CALL(userPortMock, setSmsList(Ref(expectedSmsDb)))
        .Times(AtLeast(1));
    EXPECT_CALL(userPortMock, setSelectSmsCallback(_))
        .Times(AtLeast(1));
    EXPECT_CALL(userPortMock, showSmsList())
        .Times(AtLeast(1));
    
    objectUnderTest.viewSms();
}

TEST_F(ViewSmsTestSuite, ShallInterruptSendingSmsWhenReceivingCallRequest)
{
    testing::Mock::VerifyAndClearExpectations(&userPortMock);
    testing::Mock::VerifyAndClearExpectations(&btsPortMock);
    testing::Mock::VerifyAndClearExpectations(&timerPortMock);

    EXPECT_CALL(userPortMock, showSmsComposeView());
    objectUnderTest.composeSms();
    
    testing::Mock::VerifyAndClearExpectations(&userPortMock);
    testing::Mock::VerifyAndClearExpectations(&btsPortMock);
    testing::Mock::VerifyAndClearExpectations(&timerPortMock);

    const common::PhoneNumber callerNumber{178};

    EXPECT_CALL(timerPortMock, startTimer(_));
    EXPECT_CALL(userPortMock, showCallRequest(callerNumber));

    EXPECT_CALL(userPortMock, showSmsComposeView())
        .Times(0);

    objectUnderTest.handleCallRequest(callerNumber);
    
    testing::Mock::VerifyAndClearExpectations(&userPortMock);
    testing::Mock::VerifyAndClearExpectations(&btsPortMock);
    testing::Mock::VerifyAndClearExpectations(&timerPortMock);

    EXPECT_CALL(timerPortMock, stopTimer());
    EXPECT_CALL(btsPortMock, sendCallDropped(callerNumber));
    EXPECT_CALL(userPortMock, showConnected());

    EXPECT_CALL(userPortMock, showSmsComposeView())
        .Times(0);
    
    objectUnderTest.rejectCallRequest();
    
    testing::Mock::VerifyAndClearExpectations(&userPortMock);
    testing::Mock::VerifyAndClearExpectations(&btsPortMock);
    
    const common::PhoneNumber callerNumber2{179};

    EXPECT_CALL(userPortMock, showSmsComposeView());
    objectUnderTest.composeSms();
    
    testing::Mock::VerifyAndClearExpectations(&userPortMock);
    testing::Mock::VerifyAndClearExpectations(&timerPortMock);

    EXPECT_CALL(timerPortMock, startTimer(_));
    EXPECT_CALL(userPortMock, showCallRequest(callerNumber2));

    EXPECT_CALL(userPortMock, showSmsComposeView())
        .Times(0);
    
    objectUnderTest.handleCallRequest(callerNumber2);
    
    testing::Mock::VerifyAndClearExpectations(&userPortMock);
    testing::Mock::VerifyAndClearExpectations(&timerPortMock);
    testing::Mock::VerifyAndClearExpectations(&btsPortMock);
    
    EXPECT_CALL(timerPortMock, stopTimer());
    EXPECT_CALL(btsPortMock, sendCallAccept(callerNumber2));

    EXPECT_CALL(userPortMock, showSmsComposeView())
        .Times(0);
    
    objectUnderTest.acceptCallRequest();
    
    testing::Mock::VerifyAndClearExpectations(&userPortMock);
    testing::Mock::VerifyAndClearExpectations(&btsPortMock);
    testing::Mock::VerifyAndClearExpectations(&timerPortMock);

    EXPECT_CALL(userPortMock, showConnected());

    EXPECT_CALL(userPortMock, showSmsComposeView())
        .Times(0);
    
    objectUnderTest.handleCallDropped(callerNumber2);
}
}
