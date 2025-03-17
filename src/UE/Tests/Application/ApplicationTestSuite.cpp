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

    std::unique_ptr<Application> applicationPtr;

    void SetUp() override {
        // Set up expectations for the initial connection state when app is created
        EXPECT_CALL(userPortMock, showNotConnected());
    }
    
    void createApplication() {
        applicationPtr = std::make_unique<Application>(PHONE_NUMBER, 
                                                      loggerMock,
                                                      btsPortMock, 
                                                      userPortMock, 
                                                      timerPortMock);
    }

    void verifyShowNotConnectedOnStart() {
        // Clear expectations after application start
        Mock::VerifyAndClearExpectations(&userPortMock);
    }
};

struct ApplicationNotConnectedTestSuite : ApplicationTestSuite
{};

TEST_F(ApplicationTestSuite, UeShallAttachToBtsAfterReceivingSib)
{
    // Create application with expectations already set in SetUp()
    createApplication();
    verifyShowNotConnectedOnStart();

    // GIVEN
    common::BtsId btsId{42};
    
    // WHEN - BTS sends SIB with its ID
    EXPECT_CALL(btsPortMock, sendAttachRequest(btsId));
    EXPECT_CALL(timerPortMock, startTimer(std::chrono::milliseconds{500}));
    applicationPtr->handleSib(btsId);

    // WHEN - BTS accepts attachment
    EXPECT_CALL(timerPortMock, stopTimer());
    EXPECT_CALL(userPortMock, showConnected());
    applicationPtr->handleAttachAccept();
}

TEST_F(ApplicationTestSuite, UeShallHandleAttachmentRejection)
{
    // Create application with expectations already set in SetUp()
    createApplication();
    verifyShowNotConnectedOnStart();
    
    // GIVEN
    common::BtsId btsId{42};
    
    // WHEN - BTS sends SIB and UE sends attachment request
    EXPECT_CALL(btsPortMock, sendAttachRequest(btsId));
    EXPECT_CALL(timerPortMock, startTimer(std::chrono::milliseconds{500}));
    applicationPtr->handleSib(btsId);

    // WHEN - BTS rejects attachment
    EXPECT_CALL(timerPortMock, stopTimer());
    EXPECT_CALL(userPortMock, showNotConnected());
    applicationPtr->handleAttachReject();
}

TEST_F(ApplicationTestSuite, UeShallHandleAttachmentTimeout)
{
    // Create application with expectations already set in SetUp()
    createApplication();
    verifyShowNotConnectedOnStart();
    
    // GIVEN
    common::BtsId btsId{42};
    
    // WHEN - BTS sends SIB and UE sends attachment request
    EXPECT_CALL(btsPortMock, sendAttachRequest(btsId));
    EXPECT_CALL(timerPortMock, startTimer(std::chrono::milliseconds{500}));
    applicationPtr->handleSib(btsId);

    // WHEN - BTS doesn't respond and timer expires
    EXPECT_CALL(userPortMock, showNotConnected());
    applicationPtr->handleTimeout();
}

}