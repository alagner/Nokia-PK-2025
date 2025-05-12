#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "Application.hpp"        
#include "Mocks/ILoggerMock.hpp"
#include "Mocks/IBtsPortMock.hpp"
#include "Mocks/IUserPortMock.hpp"  
#include "Mocks/ITimerPortMock.hpp"
#include "Mocks/IUeGuiMock.hpp"     
#include "Messages/PhoneNumber.hpp"
#include "Messages/BtsId.hpp"       
#include "Data/SmsData.hpp"         
#include <memory>
#include <chrono> 

namespace ue
{
using namespace ::testing;

// --- Definicje Mocków i Bazowej Fixture ---

using LoggerNiceMock = NiceMock<common::ILoggerMock>;
using BtsPortStrictMock = StrictMock<IBtsPortMock>;
using UserPortStrictMock = StrictMock<IUserPortMock>;
using TimerPortNiceMock = NiceMock<ITimerPortMock>;


class ApplicationBasicTestFixture : public Test
{
protected:
    const common::PhoneNumber PHONE_NUMBER{112}; 
    const common::BtsId BTS_ID{1024};           
    LoggerNiceMock loggerMock;
    BtsPortStrictMock btsPortMock;
    UserPortStrictMock userPortMock; 
    TimerPortNiceMock timerPortMock;

    
    
    IListViewModeMock listViewModeMock;
    ISmsComposeModeMock smsComposeModeMock;
    ITextModeMock viewTextModeMock;
    IDialModeMock dialModeMock;
    ICallModeMock callModeMock;
    ITextModeMock alertModeMock;

    Application objectUnderTest{PHONE_NUMBER,
                                loggerMock,
                                btsPortMock,
                                userPortMock,
                                timerPortMock};

    ApplicationBasicTestFixture()
    {
    }

    void simulateUserAction(const std::string& actionId) {
        objectUnderTest.handleUserAction(actionId);
    }

    void ensureConnectedState()
    {
        EXPECT_CALL(btsPortMock, sendAttachRequest(BTS_ID));
        EXPECT_CALL(timerPortMock, startTimer(std::chrono::milliseconds(500)));
        EXPECT_CALL(userPortMock, showConnecting());
        objectUnderTest.handleSib(BTS_ID);
        Mock::VerifyAndClearExpectations(&btsPortMock);
        Mock::VerifyAndClearExpectations(&timerPortMock);
        Mock::VerifyAndClearExpectations(&userPortMock);

        EXPECT_CALL(timerPortMock, stopTimer());
        EXPECT_CALL(userPortMock, showConnected());
        EXPECT_CALL(userPortMock, showNewSms(false));
        objectUnderTest.handleAttachAccept();
        Mock::VerifyAndClearExpectations(&timerPortMock);
        Mock::VerifyAndClearExpectations(&userPortMock);
    }
};

// test sprawdzający inicjalizację i stan początkowy
TEST_F(ApplicationBasicTestFixture, shallInitializeInNotConnectedStateAndHandleSib)
{
    EXPECT_CALL(btsPortMock, sendAttachRequest(BTS_ID));
    EXPECT_CALL(timerPortMock, startTimer(std::chrono::milliseconds(500))); 
    EXPECT_CALL(userPortMock, showConnecting());

    objectUnderTest.handleSib(BTS_ID); 

}

TEST_F(ApplicationBasicTestFixture, shallTransitionToConnectedStateOnAttachAccept)
{
   
    EXPECT_CALL(btsPortMock, sendAttachRequest(BTS_ID));
    EXPECT_CALL(timerPortMock, startTimer(std::chrono::milliseconds(500)));
    EXPECT_CALL(userPortMock, showConnecting());
    objectUnderTest.handleSib(BTS_ID);
    Mock::VerifyAndClearExpectations(&btsPortMock); 
    Mock::VerifyAndClearExpectations(&timerPortMock);
    Mock::VerifyAndClearExpectations(&userPortMock);

   
    EXPECT_CALL(timerPortMock, stopTimer());
    EXPECT_CALL(userPortMock, showConnected());
    EXPECT_CALL(userPortMock, showNewSms(false)); 

    objectUnderTest.handleAttachAccept();

}
// test sprawdzający obsługę rozłączenia z BTS
TEST_F(ApplicationBasicTestFixture, shallTransitionToNotConnectedOnDisconnectWhenConnected)
{
    ensureConnectedState(); 

    EXPECT_CALL(timerPortMock, stopTimer()).Times(AtLeast(1)); 
    EXPECT_CALL(userPortMock, showNotConnected());

    objectUnderTest.handleDisconnect();

    EXPECT_CALL(btsPortMock, sendAttachRequest(BTS_ID));
    EXPECT_CALL(timerPortMock, startTimer(std::chrono::milliseconds(500)));
    EXPECT_CALL(userPortMock, showConnecting());
    objectUnderTest.handleSib(BTS_ID); // Powinno znowu zainicjować proces łączenia
}


} // namespace ue
