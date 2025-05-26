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
#include "Messages/MessageId.hpp"  
#include "Data/SmsData.hpp"        
#include <memory>
#include <chrono> 
#include <vector>
#include <string>

namespace ue
{
using namespace ::testing; 

// --- Definicje Mocków i Bazowej Fixture ---
using LoggerNiceMock = NiceMock<common::ILoggerMock>;
using BtsPortStrictMock = StrictMock<IBtsPortMock>;
using UserPortStrictMock = StrictMock<IUserPortMock>; 
using TimerPortNiceMock = NiceMock<ITimerPortMock>;

class ApplicationConnectivityTestFixture : public Test
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

    
    ApplicationConnectivityTestFixture()
    {
        
    }

   
    void enterConnectingState()
    {
        EXPECT_CALL(btsPortMock, sendAttachRequest(BTS_ID));
        EXPECT_CALL(timerPortMock, startTimer(std::chrono::milliseconds(500)));
        EXPECT_CALL(userPortMock, showConnecting());
        objectUnderTest.handleSib(BTS_ID);
        Mock::VerifyAndClearExpectations(&btsPortMock);
        Mock::VerifyAndClearExpectations(&timerPortMock);
        Mock::VerifyAndClearExpectations(&userPortMock);
    }

    
    void enterConnectedState()
    {
        enterConnectingState(); 

        EXPECT_CALL(timerPortMock, stopTimer());
        EXPECT_CALL(userPortMock, showConnected());
        EXPECT_CALL(userPortMock, showNewSms(false)); 
        objectUnderTest.handleAttachAccept();
        Mock::VerifyAndClearExpectations(&timerPortMock);
        Mock::VerifyAndClearExpectations(&userPortMock);
    }
};

// Test 1: Sprawdza, czy aplikacja poprawnie inicjalizuje się w stanie NotConnected

TEST_F(ApplicationConnectivityTestFixture, shallStartInNotConnectedAndTransitionToConnectingOnSib)
{
    

    EXPECT_CALL(btsPortMock, sendAttachRequest(BTS_ID));
    EXPECT_CALL(timerPortMock, startTimer(std::chrono::milliseconds(500)));
    EXPECT_CALL(userPortMock, showConnecting());

    objectUnderTest.handleSib(BTS_ID); 


}

// Test 2: Sprawdza przejście ze stanu Connecting do Connected po otrzymaniu AttachAccept.
TEST_F(ApplicationConnectivityTestFixture, shallTransitionFromConnectingToConnectedOnAttachAccept)
{
    
    enterConnectingState();

   
    EXPECT_CALL(timerPortMock, stopTimer());
    EXPECT_CALL(userPortMock, showConnected());
    EXPECT_CALL(userPortMock, showNewSms(false));

    objectUnderTest.handleAttachAccept();

    
}

// Test 3: Sprawdza przejście ze stanu Connecting do NotConnected po otrzymaniu AttachReject.
TEST_F(ApplicationConnectivityTestFixture, shallTransitionFromConnectingToNotConnectedOnAttachReject)
{
    
    enterConnectingState();

    
    EXPECT_CALL(timerPortMock, stopTimer());
    EXPECT_CALL(userPortMock, showNotConnected());

    objectUnderTest.handleAttachReject();

    
    EXPECT_CALL(btsPortMock, sendAttachRequest(BTS_ID));
    EXPECT_CALL(timerPortMock, startTimer(std::chrono::milliseconds(500)));
    EXPECT_CALL(userPortMock, showConnecting());
    objectUnderTest.handleSib(BTS_ID);
}

// Test 4: Sprawdza obsługę timeoutu w stanie Connecting.
TEST_F(ApplicationConnectivityTestFixture, shallTransitionToNotConnectedOnTimeoutInConnectingState)
{
   
    enterConnectingState();

   
    EXPECT_CALL(timerPortMock, stopTimer()); 
    EXPECT_CALL(userPortMock, showNotConnected());

    objectUnderTest.handleTimeout(); 
   
}

// Test 5: Sprawdza obsługę rozłączenia (handleDisconnect), gdy aplikacja jest w stanie Connected.
TEST_F(ApplicationConnectivityTestFixture, shallTransitionToNotConnectedOnDisconnectWhenConnected)
{
    
    enterConnectedState();

   
    EXPECT_CALL(timerPortMock, stopTimer()).Times(AtLeast(1)); 
    EXPECT_CALL(userPortMock, showNotConnected());

    objectUnderTest.handleDisconnect();

   
    EXPECT_CALL(btsPortMock, sendAttachRequest(BTS_ID));
    EXPECT_CALL(timerPortMock, startTimer(std::chrono::milliseconds(500)));
    EXPECT_CALL(userPortMock, showConnecting());
    objectUnderTest.handleSib(BTS_ID);
}

} // namespace ue
