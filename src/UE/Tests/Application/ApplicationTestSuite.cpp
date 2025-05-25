#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "Application.hpp"
#include "Mocks/ILoggerMock.hpp"
#include "Mocks/IBtsPortMock.hpp"
#include "Mocks/IUserPortMock.hpp"
#include "Mocks/ITimerPortMock.hpp"
#include "Mocks/ISmsRepositoryMock.h"
#include "Messages/PhoneNumber.hpp"
#include <memory>
#include "Messages/OutgoingMessage.hpp"

namespace ue
{
using namespace ::testing;
using namespace std::chrono_literals;

struct ApplicationTestSuite : Test
{
    const common::PhoneNumber PHONE_NUMBER{112};
    const common::PhoneNumber TEST_SENDER_NUMBER{113};
    const common::BtsId BTS_ID{1024};
    NiceMock<common::ILoggerMock> loggerMock;
    StrictMock<IBtsPortMock> btsPortMock;
    StrictMock<IUserPortMock> userPortMock;
    StrictMock<ITimerPortMock> timerPortMock;
    StrictMock<ISmsRepositoryMock> smsRepositoryMock;

    Expectation showNotConnected = EXPECT_CALL(userPortMock, showNotConnected());
    Application objectUnderTest{PHONE_NUMBER,
                                loggerMock,
                                btsPortMock,
                                userPortMock,
                                timerPortMock,
                                smsRepositoryMock};
};

struct ApplicationNotConnectedTestSuite : ApplicationTestSuite
{
    void sendAttachRequestOnSib();
};

void ApplicationNotConnectedTestSuite::sendAttachRequestOnSib()
{
    using namespace std::chrono_literals;
    EXPECT_CALL(btsPortMock, sendAttachRequest(BTS_ID));
    EXPECT_CALL(timerPortMock, startTimer(500ms));
    EXPECT_CALL(userPortMock, showConnecting());
    objectUnderTest.handleSib(BTS_ID);
}

TEST_F(ApplicationNotConnectedTestSuite, shallSendAttachRequestOnSib)
{
    sendAttachRequestOnSib();
}

struct ApplicationConnectingTestSuite : ApplicationNotConnectedTestSuite
{
    ApplicationConnectingTestSuite()
    {
        sendAttachRequestOnSib();
    }

    void connectOnHandleAttachResponse();
};

void ApplicationConnectingTestSuite::connectOnHandleAttachResponse()
{
    EXPECT_CALL(timerPortMock, stopTimer());
    EXPECT_CALL(userPortMock, showConnected());
    objectUnderTest.handleAttachAccept();
}

TEST_F(ApplicationConnectingTestSuite, shallConnectOnAttachAccept)
{
    connectOnHandleAttachResponse();
}

TEST_F(ApplicationConnectingTestSuite, shallDisConnectOnAttachReject)
{
    EXPECT_CALL(timerPortMock, stopTimer());
    EXPECT_CALL(userPortMock, showNotConnected());
    objectUnderTest.handleAttachReject();
}

TEST_F(ApplicationConnectingTestSuite, shallDisConnectOnTimeout)
{
    EXPECT_CALL(timerPortMock, stopTimer());
    EXPECT_CALL(userPortMock, showNotConnected());
    objectUnderTest.handleTimeout();
}

TEST_F(ApplicationConnectingTestSuite, shallDisConnectOnDisConnect)
{
    EXPECT_CALL(timerPortMock, stopTimer());
    EXPECT_CALL(userPortMock, showNotConnected());
    objectUnderTest.handleDisconnect();
}

struct ApplicationConnectedTestSuite : ApplicationConnectingTestSuite
{
    ApplicationConnectedTestSuite()
    {
        connectOnHandleAttachResponse();
    }

    void showSmsListView(std::vector<SmsEntity>);
    void recieveSms(std::string);
    void sendSmsTo(common::PhoneNumber from);
};

void ApplicationConnectedTestSuite::showSmsListView(std::vector<SmsEntity> testSmsVector)
{
    EXPECT_CALL(smsRepositoryMock, getAll());
    EXPECT_CALL(userPortMock, showSmsList(testSmsVector));

    objectUnderTest.viewSmsList();
}

void ApplicationConnectedTestSuite::recieveSms(std::string text)
{
    EXPECT_CALL(userPortMock,showNewSms());
    EXPECT_CALL(smsRepositoryMock,save(_));

    objectUnderTest.handleSms(TEST_SENDER_NUMBER, text);
}

void ApplicationConnectedTestSuite::sendSmsTo(common::PhoneNumber from){
    SmsEntity smsToSend{PHONE_NUMBER.value, TEST_SENDER_NUMBER.value, "Hello!", false};

    EXPECT_CALL(userPortMock, getPhoneNumber()).WillOnce(Return(PHONE_NUMBER));
    EXPECT_CALL(smsRepositoryMock, save(smsToSend));
    EXPECT_CALL(userPortMock, showConnected());
    EXPECT_CALL(btsPortMock, sendSms(smsToSend));

    objectUnderTest.sendSms(smsToSend);
}

TEST_F(ApplicationConnectedTestSuite, shallDisConnectOnDisConnect)
{
    EXPECT_CALL(userPortMock, showNotConnected());
    objectUnderTest.handleDisconnect();
}

TEST_F(ApplicationConnectedTestSuite, shallUserReceiveNotification)
{
    recieveSms("Hello World!");
}

TEST_F(ApplicationConnectedTestSuite, sendSms_success)
{
    sendSmsTo(TEST_SENDER_NUMBER);
}

struct ApplicationSmsListViewTestSuite : ApplicationConnectedTestSuite
{
    ApplicationSmsListViewTestSuite(){
        testSmsVector.push_back({TEST_SENDER_NUMBER.value, PHONE_NUMBER.value, "test1", true});
        testSmsVector.push_back({PHONE_NUMBER.value, TEST_SENDER_NUMBER.value, "test2", true});
        testSmsVector.push_back({TEST_SENDER_NUMBER.value, PHONE_NUMBER.value, "test3"});

        smsRepositoryMock.database = testSmsVector;
    }
    std::vector<SmsEntity> testSmsVector;
};

TEST_F(ApplicationSmsListViewTestSuite, shallUserSeePopulatedSmsList)
{
    showSmsListView(testSmsVector);
}

TEST_F(ApplicationSmsListViewTestSuite, shallUserSeeEmptyList)
{
    std::vector<SmsEntity> emptyList;
    smsRepositoryMock.database.clear();
    showSmsListView(emptyList);
}

struct ApplicationSmsViewTestSuite : ApplicationSmsListViewTestSuite {};

TEST_F(ApplicationSmsViewTestSuite, shallUserReadSeenSmsAndExit)
{
    EXPECT_CALL(userPortMock, showSms(testSmsVector[0]));
    EXPECT_CALL(smsRepositoryMock, getAll());
    objectUnderTest.viewSms(0);

    showSmsListView(testSmsVector);
}

TEST_F(ApplicationSmsViewTestSuite, shallUserReadUnseenSmsAndExit)
{
    EXPECT_CALL(userPortMock, showSms(testSmsVector[2]));
    EXPECT_CALL(smsRepositoryMock, saveAll(_, true));
    EXPECT_CALL(smsRepositoryMock, getAll());
    objectUnderTest.viewSms(2);

    testSmsVector[2].isRead = true;

    showSmsListView(testSmsVector);
}

TEST_F(ApplicationSmsViewTestSuite, shallUserReadMultipleSmsAndExit)
{
    EXPECT_CALL(userPortMock, showSms(testSmsVector[0]));
    EXPECT_CALL(smsRepositoryMock, getAll());
    objectUnderTest.viewSms(0);

    showSmsListView(testSmsVector);

    EXPECT_CALL(userPortMock, showSms(testSmsVector[2]));
    EXPECT_CALL(smsRepositoryMock, saveAll(_, true));
    EXPECT_CALL(smsRepositoryMock, getAll());
    objectUnderTest.viewSms(2);

    testSmsVector[2].isRead = true;

    showSmsListView(testSmsVector);
}

struct ApplicationUnknownRecipientTestSuite : ApplicationConnectedTestSuite {
    ApplicationUnknownRecipientTestSuite(){
        sendSmsTo(TEST_SENDER_NUMBER);
        testVector = smsRepositoryMock.database;
    }
    std::vector<SmsEntity> testVector;
};

TEST_F(ApplicationUnknownRecipientTestSuite, shallMarkSmsAsFailedWhenUnknownRecipient)
{
    std::vector<SmsEntity> vectorWithFailedMessage = testVector;
    vectorWithFailedMessage[0].text="[FAILED DELIVERY] \n" + vectorWithFailedMessage[0].text;

    EXPECT_CALL(smsRepositoryMock, getAll()).WillOnce(Return(testVector));
    EXPECT_CALL(smsRepositoryMock, saveAll(vectorWithFailedMessage,true));
    objectUnderTest.handleSmsDeliveryFailure(PHONE_NUMBER);
}

struct ApplicationTalkingTestSuite : ApplicationConnectedTestSuite
{
    ApplicationTalkingTestSuite()
    {
        EXPECT_CALL(timerPortMock, stopTimer());
        EXPECT_CALL(userPortMock, showTalking(_));
        objectUnderTest.handleCallAccepted(TEST_SENDER_NUMBER);
    }
};

TEST_F(ApplicationTalkingTestSuite, shallSendTalkMessageAndRestartTimer)
{
    std::string message = "Hello!";
    EXPECT_CALL(timerPortMock, stopTimer());
    using namespace std::chrono;
    EXPECT_CALL(timerPortMock, startTimer(duration_cast<milliseconds>(minutes(2))));
    EXPECT_CALL(btsPortMock, sendTalkMessage(TEST_SENDER_NUMBER, message));

    objectUnderTest.sendTalkMessage(message);
}

TEST_F(ApplicationTalkingTestSuite, shallHandleTalkMessageAndRestartTimer)
{
    std::string message = "Hi!";
    EXPECT_CALL(userPortMock, displayMessage(TEST_SENDER_NUMBER, message));
    EXPECT_CALL(timerPortMock, stopTimer());
    using namespace std::chrono;
    EXPECT_CALL(timerPortMock, startTimer(duration_cast<milliseconds>(minutes(2))));

    objectUnderTest.handleTalkMessage(TEST_SENDER_NUMBER, message);
}

TEST_F(ApplicationTalkingTestSuite, shallHandleTimeoutAndDropCall)
{
    EXPECT_CALL(userPortMock, getPhoneNumber()).WillOnce(Return(PHONE_NUMBER));
    EXPECT_CALL(btsPortMock, sendCallDropped(PHONE_NUMBER, TEST_SENDER_NUMBER));
    EXPECT_CALL(userPortMock, showConnected()).Times(AtLeast(1));
    objectUnderTest.handleTimeout();
}

TEST_F(ApplicationConnectedTestSuite, shallGoToTalkingStateWhenCallAccepted)
{
    EXPECT_CALL(userPortMock, getPhoneNumber()).WillOnce(Return(PHONE_NUMBER));
    EXPECT_CALL(btsPortMock, sendCallRequest(PHONE_NUMBER, TEST_SENDER_NUMBER));
    EXPECT_CALL(timerPortMock, startTimer(_));
    EXPECT_CALL(userPortMock, showDialing());

    objectUnderTest.sendCallRequest(TEST_SENDER_NUMBER);

    EXPECT_CALL(timerPortMock, stopTimer());
    EXPECT_CALL(userPortMock, showTalking(_));
    objectUnderTest.handleCallAccepted(PHONE_NUMBER);
}

TEST_F(ApplicationConnectedTestSuite, shallReturnToConnectedOnCallTimeout)
{
    EXPECT_CALL(userPortMock, getPhoneNumber()).Times(AnyNumber()).WillRepeatedly(Return(PHONE_NUMBER));

    EXPECT_CALL(btsPortMock, sendCallRequest(PHONE_NUMBER, TEST_SENDER_NUMBER));
    EXPECT_CALL(timerPortMock, startTimer(_));
    EXPECT_CALL(userPortMock, showDialing());

    objectUnderTest.sendCallRequest(TEST_SENDER_NUMBER);

    EXPECT_CALL(btsPortMock, sendCallDropped(PHONE_NUMBER, TEST_SENDER_NUMBER));
    EXPECT_CALL(userPortMock, showConnected());

    objectUnderTest.handleTimeout();
}

TEST_F(ApplicationTalkingTestSuite, shallHandlePeerUnavailable)
{
    EXPECT_CALL(userPortMock, showPartnerNotAvailable(TEST_SENDER_NUMBER));
    EXPECT_CALL(timerPortMock, stopTimer());
    using namespace std::chrono;
    EXPECT_CALL(timerPortMock, startRedirectTimer(duration_cast<milliseconds>(seconds(3))));
    objectUnderTest.handleCallRecipientNotAvailable();
}

TEST_F(ApplicationConnectedTestSuite, shallReturnToConnectedOnUnknownRecipient)
{
    EXPECT_CALL(userPortMock, getPhoneNumber()).WillOnce(Return(PHONE_NUMBER));
    EXPECT_CALL(btsPortMock, sendCallRequest(PHONE_NUMBER, TEST_SENDER_NUMBER));
    EXPECT_CALL(timerPortMock, startTimer(_));
    EXPECT_CALL(userPortMock, showDialing());

    objectUnderTest.sendCallRequest(TEST_SENDER_NUMBER);

    EXPECT_CALL(timerPortMock, stopTimer());
    EXPECT_CALL(timerPortMock, startRedirectTimer(_));
    EXPECT_CALL(userPortMock, showPartnerNotAvailable(TEST_SENDER_NUMBER));

    objectUnderTest.handleCallRecipientNotAvailable();
}

TEST_F(ApplicationConnectedTestSuite, shallReturnToConnectedOnCallDroppedFromBts)
{
    EXPECT_CALL(userPortMock, getPhoneNumber()).WillRepeatedly(Return(PHONE_NUMBER));
    EXPECT_CALL(btsPortMock, sendCallRequest(PHONE_NUMBER, TEST_SENDER_NUMBER));
    EXPECT_CALL(timerPortMock, startTimer(_));
    EXPECT_CALL(userPortMock, showDialing());

    objectUnderTest.sendCallRequest(TEST_SENDER_NUMBER);

    EXPECT_CALL(timerPortMock, stopTimer());
    EXPECT_CALL(userPortMock, showConnected());

    objectUnderTest.handleCallDropped();
}

TEST_F(ApplicationConnectedTestSuite, shallResignCallRequestWhenUserCancels)
{
    EXPECT_CALL(userPortMock, getPhoneNumber()).WillRepeatedly(Return(PHONE_NUMBER));
    EXPECT_CALL(btsPortMock, sendCallRequest(PHONE_NUMBER, TEST_SENDER_NUMBER));
    EXPECT_CALL(timerPortMock, startTimer(_));
    EXPECT_CALL(userPortMock, showDialing());

    objectUnderTest.sendCallRequest(TEST_SENDER_NUMBER);

    EXPECT_CALL(timerPortMock, stopTimer());
    EXPECT_CALL(btsPortMock, sendCallDropped(PHONE_NUMBER, TEST_SENDER_NUMBER));
    EXPECT_CALL(userPortMock, showConnected());

    objectUnderTest.cancelCallRequest();
}

TEST_F(ApplicationTalkingTestSuite, shallHandleUserDroppingCall)
{
    EXPECT_CALL(userPortMock, getPhoneNumber()).WillOnce(Return(PHONE_NUMBER));
    EXPECT_CALL(btsPortMock, sendCallDropped(PHONE_NUMBER, TEST_SENDER_NUMBER));
    EXPECT_CALL(timerPortMock, stopTimer()).Times(AtLeast(1));
    EXPECT_CALL(userPortMock, showConnected()).Times(AtLeast(1));
    objectUnderTest.callDrop();
}

}
