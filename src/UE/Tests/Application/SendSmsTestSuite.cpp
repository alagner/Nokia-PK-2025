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
#include <string>

namespace ue
{
using namespace ::testing;

class SendSmsTestSuite : public Test
{
protected:
    const common::PhoneNumber PHONE_NUMBER{42};
    const common::PhoneNumber RECIPIENT_NUMBER{123};
    const common::PhoneNumber CALLING_NUMBER{200};
    const common::BtsId BTS_ID{1};
    const std::string SMS_TEXT = "Hello World!";

    NiceMock<common::ILoggerMock> loggerMock;
    StrictMock<IBtsPortMock> btsPortMock;
    StrictMock<IUserPortMock> userPortMock;
    StrictMock<ITimerPortMock> timerPortMock;
    StrictMock<ISmsDbMock> smsDbMock;

    Application objectUnderTest;

    SendSmsTestSuite()
        : objectUnderTest(PHONE_NUMBER, 
                         loggerMock,
                         btsPortMock,
                         userPortMock,
                         timerPortMock,
                         smsDbMock)
    {}

    void SetUp() override
    {
        // Start in Connected state
        EXPECT_CALL(userPortMock, showConnecting());
        EXPECT_CALL(btsPortMock, sendAttachRequest(BTS_ID));
        objectUnderTest.handleSib(BTS_ID);
        
        EXPECT_CALL(userPortMock, showConnected());
        objectUnderTest.handleAttachAccept();
    }

    void enterSmsComposeState()
    {
        EXPECT_CALL(userPortMock, showSmsComposeView());
        objectUnderTest.composeSms();
    }

    void expectSmsSuccessfullySent(const common::PhoneNumber& recipient, const std::string& text)
    {
        EXPECT_CALL(btsPortMock, sendSms(recipient, text));
        EXPECT_CALL(smsDbMock, addSentSms(PHONE_NUMBER, recipient, text));
        EXPECT_CALL(userPortMock, showConnected());
    }

    void expectSmsCompositionRejected()
    {
        EXPECT_CALL(userPortMock, showConnected());
    }
};



}