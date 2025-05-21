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

class ApplicationTestSuite : public Test
{
protected:
    const common::PhoneNumber PHONE_NUMBER{112};
    NiceMock<common::ILoggerMock> loggerMock;
    StrictMock<IBtsPortMock> btsPortMock;
    StrictMock<IUserPortMock> userPortMock;
    StrictMock<ITimerPortMock> timerPortMock;

    Application objectUnderTest{PHONE_NUMBER,
                                loggerMock,
                                btsPortMock,
                                userPortMock,
                                timerPortMock};
};

struct ApplicationNotConnectedTestSuite : ApplicationTestSuite
{};

TEST_F(ApplicationNotConnectedTestSuite, ShallSendAttachRequestOnSibReceived)
{
    // When SIB is received, the application should:
    // 1. Send an attach request with the received BTS ID
    // 2. Start a timer
    const common::BtsId btsId = 123;
    
    EXPECT_CALL(btsPortMock, sendAttachRequest(btsId));
    EXPECT_CALL(timerPortMock, startTimer(_));
    
    objectUnderTest.handleSib(btsId);
}

TEST_F(ApplicationNotConnectedTestSuite, ShallIgnoreIncomingSms)
{
    // In NotConnected state, incoming SMS should be ignored
    // No calls to userPortMock should be made
    
    const common::PhoneNumber senderNumber{200};
    const std::string smsText = "Hello, test message";
    
    // No expectations set on any mock - if any method is called, test will fail
    
    objectUnderTest.handleSms(senderNumber, smsText);
}

struct ApplicationConnectingTestSuite : ApplicationTestSuite
{
    ApplicationConnectingTestSuite()
    {
        // Start in connecting state by handling SIB first
        EXPECT_CALL(btsPortMock, sendAttachRequest(_));
        EXPECT_CALL(timerPortMock, startTimer(_));
        EXPECT_CALL(userPortMock, showConnecting()); // ConnectingState constructor shows "connecting"
        objectUnderTest.handleSib(123);
    }
};

TEST_F(ApplicationConnectingTestSuite, ShallShowConnectedOnAttachAccept)
{
    // When attach is accepted, the application should:
    // 1. Stop the timer
    // 2. Transition to ConnectedState (which will show "connected")
    
    EXPECT_CALL(timerPortMock, stopTimer());
    EXPECT_CALL(userPortMock, showConnected());
    
    objectUnderTest.handleAttachAccept();
}

TEST_F(ApplicationConnectingTestSuite, ShallShowNotConnectedOnAttachReject)
{
    // When attach is rejected, the application should:
    // 1. Stop the timer
    // 2. Transition to NotConnectedState (which will show "not connected")
    
    EXPECT_CALL(timerPortMock, stopTimer());
    EXPECT_CALL(userPortMock, showNotConnected());
    
    objectUnderTest.handleAttachReject();
}

TEST_F(ApplicationConnectingTestSuite, ShallShowNotConnectedOnTimeout)
{
    // When timeout occurs during connection attempt, the application should:
    // 1. Transition to NotConnectedState (which will show "not connected")
    
    EXPECT_CALL(userPortMock, showNotConnected());
    
    objectUnderTest.handleTimeout();
}

TEST_F(ApplicationConnectingTestSuite, ShallShowNotConnectedOnDisconnect)
{
    // When disconnect occurs during connection attempt, the application should:
    // 1. Transition to NotConnectedState (which will show "not connected")
    
    EXPECT_CALL(userPortMock, showNotConnected());
    
    objectUnderTest.handleDisconnect();
}

TEST_F(ApplicationConnectingTestSuite, ShallIgnoreIncomingSms)
{
    // In Connecting state, incoming SMS should be ignored
    const common::PhoneNumber senderNumber{200};
    const std::string smsText = "Hello, test message";
    
    // No expectations set - if any method is called, test will fail
    
    objectUnderTest.handleSms(senderNumber, smsText);
}

struct ApplicationConnectedTestSuite : ApplicationTestSuite
{
    ApplicationConnectedTestSuite()
    {
        // Start in connecting state by handling SIB first
        EXPECT_CALL(btsPortMock, sendAttachRequest(_));
        EXPECT_CALL(timerPortMock, startTimer(_));
        objectUnderTest.handleSib(123);
        
        // Then move to connected state by handling AttachAccept
        EXPECT_CALL(timerPortMock, stopTimer());
        EXPECT_CALL(userPortMock, showConnected());
        objectUnderTest.handleAttachAccept();
    }
};

TEST_F(ApplicationConnectedTestSuite, ShallShowNotConnectedOnDisconnect)
{
    // When disconnect occurs in connected state, the application should:
    // 1. Transition to NotConnectedState (which will show "not connected")
    
    EXPECT_CALL(userPortMock, showNotConnected());
    
    objectUnderTest.handleDisconnect();
}

TEST_F(ApplicationConnectedTestSuite, ShallHandleIncomingSms)
{
    // In Connected state, incoming SMS should update the SMS database and show notification
    const common::PhoneNumber senderNumber{200};
    const std::string smsText = "Hello, test message";
    
    EXPECT_CALL(userPortMock, showNewSms(true));
    
    objectUnderTest.handleSms(senderNumber, smsText);
}

TEST_F(ApplicationConnectedTestSuite, ShallHandleCallRequest)
{
    // In Connected state, call request should show call request UI and start a timer
    const common::PhoneNumber callerNumber{300};
    
    EXPECT_CALL(userPortMock, showCallRequest(callerNumber));
    EXPECT_CALL(timerPortMock, startTimer(_));
    
    objectUnderTest.handleCallRequest(callerNumber);
}

TEST_F(ApplicationConnectedTestSuite, ShallHandleCallRequestAccepted)
{
    // Setup - receive a call request first
    const common::PhoneNumber callerNumber{300};
    EXPECT_CALL(userPortMock, showCallRequest(callerNumber));
    EXPECT_CALL(timerPortMock, startTimer(_));
    objectUnderTest.handleCallRequest(callerNumber);
    
    // Test - accept the call request
    EXPECT_CALL(timerPortMock, stopTimer());
    EXPECT_CALL(btsPortMock, sendCallAccept(callerNumber));
    EXPECT_CALL(userPortMock, showCallTalk(callerNumber)); // TalkingState constructor shows this
    
    objectUnderTest.acceptCallRequest();
}

TEST_F(ApplicationConnectedTestSuite, ShallHandleCallRequestRejected)
{
    // Setup - receive a call request first
    const common::PhoneNumber callerNumber{300};
    EXPECT_CALL(userPortMock, showCallRequest(callerNumber));
    EXPECT_CALL(timerPortMock, startTimer(_));
    objectUnderTest.handleCallRequest(callerNumber);
    
    // Test - reject the call request
    EXPECT_CALL(timerPortMock, stopTimer());
    EXPECT_CALL(btsPortMock, sendCallDropped(callerNumber));
    EXPECT_CALL(userPortMock, showConnected());
    
    objectUnderTest.rejectCallRequest();
}

TEST_F(ApplicationConnectedTestSuite, ShallAutoRejectCallRequestOnTimeout)
{
    // Setup - receive a call request first
    const common::PhoneNumber callerNumber{300};
    EXPECT_CALL(userPortMock, showCallRequest(callerNumber));
    EXPECT_CALL(timerPortMock, startTimer(_));
    objectUnderTest.handleCallRequest(callerNumber);
    
    // Test - timeout occurs
    EXPECT_CALL(btsPortMock, sendCallDropped(callerNumber));
    EXPECT_CALL(userPortMock, showConnected());
    
    objectUnderTest.handleTimeout();
}

TEST_F(ApplicationConnectedTestSuite, ShallHandleDial)
{
    // When user initiates dialing, the application should transition to DialState
    EXPECT_CALL(userPortMock, showDialView());
    
    objectUnderTest.dial();
}

struct ApplicationDialTestSuite : ApplicationTestSuite
{
    ApplicationDialTestSuite()
    {
        // Start in connecting state by handling SIB first
        EXPECT_CALL(btsPortMock, sendAttachRequest(_));
        EXPECT_CALL(timerPortMock, startTimer(_));
        objectUnderTest.handleSib(123);
        
        // Then move to connected state by handling AttachAccept
        EXPECT_CALL(timerPortMock, stopTimer());
        EXPECT_CALL(userPortMock, showConnected());
        objectUnderTest.handleAttachAccept();
        
        // Finally move to dial state
        EXPECT_CALL(userPortMock, showDialView());
        objectUnderTest.dial();
    }
    
    const common::PhoneNumber RECIPIENT_NUMBER{400};
};

TEST_F(ApplicationDialTestSuite, ShallHandleAcceptDialing)
{
    // When accepting dialing, the application should:
    // 1. Start a timer for the call request
    // 2. Send a call request to the recipient
    
    EXPECT_CALL(timerPortMock, startTimer(_));
    EXPECT_CALL(btsPortMock, sendCallRequest(RECIPIENT_NUMBER));
    
    objectUnderTest.acceptDialing(RECIPIENT_NUMBER);
}

TEST_F(ApplicationDialTestSuite, ShallHandleRejectDialingWithoutRecipient)
{
    // When rejecting dialing without having chosen a recipient yet, the application should:
    // 1. Not send any call dropped message (no recipient yet)
    // 2. Transition back to ConnectedState
    
    EXPECT_CALL(userPortMock, showConnected());
    
    objectUnderTest.rejectDialing();
}

TEST_F(ApplicationDialTestSuite, ShallHandleRejectDialingWithRecipient)
{
    // Setup - first accept dialing with a recipient number
    EXPECT_CALL(timerPortMock, startTimer(_));
    EXPECT_CALL(btsPortMock, sendCallRequest(RECIPIENT_NUMBER));
    objectUnderTest.acceptDialing(RECIPIENT_NUMBER);
    
    // Test - when rejecting dialing with a chosen recipient, the application should:
    // 1. Stop the timer
    // 2. Send call dropped to the recipient
    // 3. Transition back to ConnectedState
    
    EXPECT_CALL(timerPortMock, stopTimer());
    EXPECT_CALL(btsPortMock, sendCallDropped(RECIPIENT_NUMBER));
    EXPECT_CALL(userPortMock, showConnected());
    
    objectUnderTest.rejectDialing();
}

TEST_F(ApplicationDialTestSuite, ShallHandleCallAcceptedFromRecipient)
{
    // Setup - first accept dialing with a recipient number
    EXPECT_CALL(timerPortMock, startTimer(_));
    EXPECT_CALL(btsPortMock, sendCallRequest(RECIPIENT_NUMBER));
    objectUnderTest.acceptDialing(RECIPIENT_NUMBER);
    
    // Test - when the recipient accepts the call, the application should:
    // 1. Stop the timer
    // 2. Transition to TalkingState (which will show call talk UI)
    
    EXPECT_CALL(timerPortMock, stopTimer());
    EXPECT_CALL(userPortMock, showCallTalk(RECIPIENT_NUMBER));
    
    objectUnderTest.handleCallAccepted(RECIPIENT_NUMBER);
}

TEST_F(ApplicationDialTestSuite, ShallIgnoreCallAcceptedFromWrongNumber)
{
    // Setup - first accept dialing with a recipient number
    EXPECT_CALL(timerPortMock, startTimer(_));
    EXPECT_CALL(btsPortMock, sendCallRequest(RECIPIENT_NUMBER));
    objectUnderTest.acceptDialing(RECIPIENT_NUMBER);
    
    // Test - when a different number accepts the call, the application should ignore it
    // No expectations set - if any method is called, test will fail
    
    const common::PhoneNumber WRONG_NUMBER{500};
    objectUnderTest.handleCallAccepted(WRONG_NUMBER);
}

TEST_F(ApplicationDialTestSuite, ShallHandleCallDroppedByRecipient)
{
    // Setup - first accept dialing with a recipient number
    EXPECT_CALL(timerPortMock, startTimer(_));
    EXPECT_CALL(btsPortMock, sendCallRequest(RECIPIENT_NUMBER));
    objectUnderTest.acceptDialing(RECIPIENT_NUMBER);
    
    // Test - when the recipient drops the call, the application should:
    // 1. Stop the timer
    // 2. Show "peer not available" UI
    // 3. Transition back to ConnectedState
    
    EXPECT_CALL(timerPortMock, stopTimer());
    EXPECT_CALL(userPortMock, showPeerUserNotAvailable(RECIPIENT_NUMBER));
    EXPECT_CALL(userPortMock, showConnected());
    
    objectUnderTest.handleCallDropped(RECIPIENT_NUMBER);
}

TEST_F(ApplicationDialTestSuite, ShallIgnoreCallDroppedFromWrongNumber)
{
    // Setup - first accept dialing with a recipient number
    EXPECT_CALL(timerPortMock, startTimer(_));
    EXPECT_CALL(btsPortMock, sendCallRequest(RECIPIENT_NUMBER));
    objectUnderTest.acceptDialing(RECIPIENT_NUMBER);
    
    // Test - when a different number drops the call, the application should ignore it
    // No expectations set - if any method is called, test will fail
    
    const common::PhoneNumber WRONG_NUMBER{500};
    objectUnderTest.handleCallDropped(WRONG_NUMBER);
}

TEST_F(ApplicationDialTestSuite, ShallHandleTimeout)
{
    // Setup - first accept dialing with a recipient number
    EXPECT_CALL(timerPortMock, startTimer(_));
    EXPECT_CALL(btsPortMock, sendCallRequest(RECIPIENT_NUMBER));
    objectUnderTest.acceptDialing(RECIPIENT_NUMBER);
    
    // Test - when timeout occurs, the application should:
    // 1. Show "peer not available" UI
    // 2. Transition back to ConnectedState
    
    EXPECT_CALL(userPortMock, showPeerUserNotAvailable(RECIPIENT_NUMBER));
    EXPECT_CALL(userPortMock, showConnected());
    
    objectUnderTest.handleTimeout();
}

struct ApplicationTalkingTestSuite : ApplicationTestSuite
{
    ApplicationTalkingTestSuite()
    {
        // Start in connecting state by handling SIB first
        EXPECT_CALL(btsPortMock, sendAttachRequest(_));
        EXPECT_CALL(timerPortMock, startTimer(_));
        objectUnderTest.handleSib(123);
        
        // Then move to connected state by handling AttachAccept
        EXPECT_CALL(timerPortMock, stopTimer());
        EXPECT_CALL(userPortMock, showConnected());
        objectUnderTest.handleAttachAccept();
        
        // Setup incoming call request
        EXPECT_CALL(userPortMock, showCallRequest(PEER_NUMBER));
        EXPECT_CALL(timerPortMock, startTimer(_));
        objectUnderTest.handleCallRequest(PEER_NUMBER);
        
        // Accept the call to enter TalkingState
        EXPECT_CALL(timerPortMock, stopTimer());
        EXPECT_CALL(btsPortMock, sendCallAccept(PEER_NUMBER));
        EXPECT_CALL(userPortMock, showCallTalk(PEER_NUMBER));
        objectUnderTest.acceptCallRequest();
    }
    
    const common::PhoneNumber PEER_NUMBER{300};
};

TEST_F(ApplicationTalkingTestSuite, ShallHandleCallTalkMessage)
{
    // When call talk message is received from the peer, it should be shown to the user
    const std::string message = "Hello, how are you?";
    
    EXPECT_CALL(userPortMock, showCallMessage(message));
    
    objectUnderTest.handleCallTalk(PEER_NUMBER, message);
}

TEST_F(ApplicationTalkingTestSuite, ShallIgnoreCallTalkMessageFromWrongNumber)
{
    // If message comes from wrong number, it should be ignored
    const std::string message = "Wrong number message";
    const common::PhoneNumber wrongNumber{500};
    
    // No expectations - if any method is called, test will fail
    
    objectUnderTest.handleCallTalk(wrongNumber, message);
}

TEST_F(ApplicationTalkingTestSuite, ShallHandleCallDroppedFromPeer)
{
    // When peer drops the call, the application should:
    // 1. Show "peer not available" UI
    // 2. Transition to ConnectedState
    
    EXPECT_CALL(userPortMock, showPeerUserNotAvailable(PEER_NUMBER));
    EXPECT_CALL(userPortMock, showConnected());
    
    objectUnderTest.handleCallDropped(PEER_NUMBER);
}

TEST_F(ApplicationTalkingTestSuite, ShallIgnoreCallDroppedFromWrongNumber)
{
    // If call dropped comes from wrong number, it should be ignored
    const common::PhoneNumber wrongNumber{500};
    
    // No expectations - if any method is called, test will fail
    
    objectUnderTest.handleCallDropped(wrongNumber);
}

TEST_F(ApplicationTalkingTestSuite, ShallHandleDisconnect)
{
    // When network disconnects during a call, the application should:
    // 1. Transition to NotConnectedState
    
    EXPECT_CALL(userPortMock, showNotConnected());
    
    objectUnderTest.handleDisconnect();
}

struct ApplicationSmsTestSuite : ApplicationConnectedTestSuite
{
    // This test suite inherits from ApplicationConnectedTestSuite
    // so we're already in the Connected state
};

TEST_F(ApplicationSmsTestSuite, ShallHandleViewSms)
{
    // When user requests to view SMS list, the application should
    // show the SMS list UI
    
    EXPECT_CALL(userPortMock, showSmsList());
    
    objectUnderTest.viewSms();
}

TEST_F(ApplicationSmsTestSuite, ShallHandleComposeSms)
{
    // When user requests to compose an SMS, the application should
    // show the SMS compose view
    
    EXPECT_CALL(userPortMock, showSmsComposeView());
    
    objectUnderTest.composeSms();
}

TEST_F(ApplicationSmsTestSuite, ShallHandleAcceptSmsCompose)
{
    // When user accepts SMS composition, the application should:
    // 1. Send the SMS to the recipient
    // 2. Add the sent SMS to the database
    // 3. Show connected view again
    
    const common::PhoneNumber recipientNumber{400};
    const std::string smsText = "Hello, this is a test SMS";
    
    EXPECT_CALL(btsPortMock, sendSms(recipientNumber, smsText));
    EXPECT_CALL(userPortMock, showConnected());
    EXPECT_CALL(userPortMock, showNewSms(_));
    
    objectUnderTest.acceptSmsCompose(recipientNumber, smsText);
}

TEST_F(ApplicationSmsTestSuite, ShallHandleRejectSmsCompose)
{
    // When user rejects SMS composition, the application should
    // return to connected view
    
    EXPECT_CALL(userPortMock, showConnected());
    EXPECT_CALL(userPortMock, showNewSms(_));
    
    objectUnderTest.rejectSmsCompose();
}

TEST_F(ApplicationSmsTestSuite, ShallHandleCloseSmsView)
{
    // First view the SMS list
    EXPECT_CALL(userPortMock, showSmsList());
    objectUnderTest.viewSms();
    
    // When user closes SMS view, the application should
    // return to connected view
    
    EXPECT_CALL(userPortMock, showConnected());
    EXPECT_CALL(userPortMock, showNewSms(_));
    
    objectUnderTest.closeSmsView();
}

}