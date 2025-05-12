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

class ApplicationMiscTestFixture : public Test
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

public:
    ApplicationMiscTestFixture()
    {
       
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

// Test 1: Sprawdza, czy aplikacja zwraca poprawny numer telefonu.
TEST_F(ApplicationMiscTestFixture, shallReturnOwnPhoneNumber)
{
    EXPECT_EQ(objectUnderTest.getOwnPhoneNumber(), PHONE_NUMBER);
}

// Test 2: Sprawdza, czy aplikacja udostępnia poprawny kontekst.
TEST_F(ApplicationMiscTestFixture, shallProvideValidContext)
{
    Context& context = objectUnderTest.getContext();

   
    EXPECT_EQ(&context.logger, &loggerMock);
    EXPECT_EQ(&context.bts, &btsPortMock);
    EXPECT_EQ(&context.user, &userPortMock);
    EXPECT_EQ(&context.timer, &timerPortMock);
    EXPECT_EQ(&context.app, &objectUnderTest);
    ASSERT_NE(context.state, nullptr); 
}

// Test 3: Sprawdza logikę oznaczania SMS jako przeczytany i aktualizację wskaźnika.
TEST_F(ApplicationMiscTestFixture, shallMarkSmsAsReadAndUpdateIndicator)
{
    ensureConnectedState();

    const common::PhoneNumber sender1{123}; 
    const std::string smsText1 = "First unread SMS";
    const common::PhoneNumber sender2{124}; 
    const std::string smsText2 = "Second unread SMS";

    
    EXPECT_CALL(userPortMock, showNewSms(true));
    objectUnderTest.handleSms(sender1, smsText1);
    Mock::VerifyAndClearExpectations(&userPortMock);

   
    EXPECT_CALL(userPortMock, showNewSms(true)); 
    objectUnderTest.handleSms(sender2, smsText2);
    Mock::VerifyAndClearExpectations(&userPortMock);

    auto& smsDb = objectUnderTest.getSmsDb();
    ASSERT_EQ(smsDb.size(), 2);
    EXPECT_FALSE(smsDb[0].isRead);
    EXPECT_FALSE(smsDb[1].isRead);

   
    EXPECT_CALL(userPortMock, showNewSms(true));
    objectUnderTest.markSmsAsRead(0); 
    Mock::VerifyAndClearExpectations(&userPortMock);

    EXPECT_TRUE(smsDb[0].isRead);
    EXPECT_FALSE(smsDb[1].isRead); 

    
    EXPECT_CALL(userPortMock, showNewSms(false));
    objectUnderTest.markSmsAsRead(1);
    Mock::VerifyAndClearExpectations(&userPortMock);

    EXPECT_TRUE(smsDb[0].isRead); 
    EXPECT_TRUE(smsDb[1].isRead); 
}

} // namespace ue
