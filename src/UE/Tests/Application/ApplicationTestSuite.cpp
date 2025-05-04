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
};

void ApplicationConnectedTestSuite::showSmsListView(std::vector<SmsEntity> testSmsVector)
{
    ON_CALL(smsRepositoryMock, getAll()).WillByDefault(Return(testSmsVector));

    EXPECT_CALL(smsRepositoryMock, getAll());
    EXPECT_CALL(userPortMock, showSmsList(testSmsVector));

    objectUnderTest.viewSmsList();
}

TEST_F(ApplicationConnectedTestSuite, shallDisConnectOnDisConnect)
{
    EXPECT_CALL(userPortMock, showNotConnected());
    objectUnderTest.handleDisconnect();
}

TEST_F(ApplicationConnectedTestSuite, shallUserReceiveNotification)
{
    EXPECT_CALL(userPortMock,showNewSms());
    EXPECT_CALL(smsRepositoryMock,save(_));
    objectUnderTest.handleSms(TEST_SENDER_NUMBER,"Hello World!");
}

struct ApplicationSmsListViewTestSuite : ApplicationConnectedTestSuite
{
    void showPopulatedSmsList();
    std::vector<SmsEntity> testSmsVector;
};

void ApplicationSmsListViewTestSuite::showPopulatedSmsList(){
    testSmsVector.push_back({TEST_SENDER_NUMBER.value, PHONE_NUMBER.value, "test1", true});
    testSmsVector.push_back({PHONE_NUMBER.value, TEST_SENDER_NUMBER.value, "test2", true});
    testSmsVector.push_back({TEST_SENDER_NUMBER.value, PHONE_NUMBER.value, "test3"});

    showSmsListView(testSmsVector);
}

TEST_F(ApplicationSmsListViewTestSuite, shallUserSeePopulatedSmsList)
{
    showPopulatedSmsList();
}

TEST_F(ApplicationSmsListViewTestSuite, shallUserSeeEmptyList)
{
    showSmsListView(testSmsVector);
}

struct ApplicationSmsViewTestSuite : ApplicationSmsListViewTestSuite
{
    ApplicationSmsViewTestSuite(){
        showPopulatedSmsList();
    }
};

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

    EXPECT_CALL(userPortMock, showSms(testSmsVector[1]));
    EXPECT_CALL(smsRepositoryMock, getAll());
    objectUnderTest.viewSms(1);

    showSmsListView(testSmsVector);
}

}
