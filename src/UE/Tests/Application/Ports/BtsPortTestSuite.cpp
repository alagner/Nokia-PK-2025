#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "Ports/BtsPort.hpp"
#include "Mocks/ILoggerMock.hpp"
#include "Mocks/IBtsPortMock.hpp"
#include "Messages/PhoneNumber.hpp"
#include "Mocks/ITransportMock.hpp"
#include "Messages/OutgoingMessage.hpp"
#include "Messages/IncomingMessage.hpp"

namespace ue
{
using namespace ::testing;

class BtsPortTestSuite : public Test
{
protected:
    const common::PhoneNumber PHONE_NUMBER{112};
    const common::BtsId BTS_ID{13121981ll};
    NiceMock<common::ILoggerMock> loggerMock;
    StrictMock<IBtsEventsHandlerMock> handlerMock;
    StrictMock<common::ITransportMock> transportMock;
    common::ITransport::MessageCallback messageCallback;
    common::ITransport::DisconnectedCallback disconnectedCallback;

    BtsPort objectUnderTest{loggerMock, transportMock, PHONE_NUMBER};

    BtsPortTestSuite()
    {
        EXPECT_CALL(transportMock, registerMessageCallback(_))
                .WillOnce(SaveArg<0>(&messageCallback));
        EXPECT_CALL(transportMock, registerDisconnectedCallback(_))
                .WillOnce(SaveArg<0>(&disconnectedCallback));
        objectUnderTest.start(handlerMock);
    }
    ~BtsPortTestSuite()
    {

        EXPECT_CALL(transportMock, registerMessageCallback(IsNull()));
        EXPECT_CALL(transportMock, registerDisconnectedCallback(IsNull()));
        objectUnderTest.stop();
    }
};

TEST_F(BtsPortTestSuite, shallRegisterHandlersBetweenStartStop)
{
}

TEST_F(BtsPortTestSuite, shallIgnoreWrongMessage)
{
    common::OutgoingMessage wrongMsg{};
    wrongMsg.writeBtsId(BTS_ID);
    messageCallback(wrongMsg.getMessage());
}

TEST_F(BtsPortTestSuite, shallHandleSib)
{
    EXPECT_CALL(handlerMock, handleSib(BTS_ID));
    common::OutgoingMessage msg{common::MessageId::Sib,
                                common::PhoneNumber{},
                                PHONE_NUMBER};
    msg.writeBtsId(BTS_ID);
    messageCallback(msg.getMessage());
}

TEST_F(BtsPortTestSuite, shallHandleAttachAccept)
{
    EXPECT_CALL(handlerMock, handleAttachAccept());
    common::OutgoingMessage msg{common::MessageId::AttachResponse,
                                common::PhoneNumber{},
                                PHONE_NUMBER};
    msg.writeNumber(true);
    messageCallback(msg.getMessage());
}

TEST_F(BtsPortTestSuite, shallHandleAttachReject)
{
    EXPECT_CALL(handlerMock, handleAttachReject());
    common::OutgoingMessage msg{common::MessageId::AttachResponse,
                                common::PhoneNumber{},
                                PHONE_NUMBER};
    msg.writeNumber(false);
    messageCallback(msg.getMessage());
}

TEST_F(BtsPortTestSuite, shallSendAttachRequest)
{
    common::BinaryMessage msg;
    EXPECT_CALL(transportMock, sendMessage(_)).WillOnce([&msg](auto param) { msg = std::move(param); return true; });
    objectUnderTest.sendAttachRequest(BTS_ID);
    common::IncomingMessage reader(msg);
    ASSERT_NO_THROW(EXPECT_EQ(common::MessageId::AttachRequest, reader.readMessageId()) );
    ASSERT_NO_THROW(EXPECT_EQ(PHONE_NUMBER, reader.readPhoneNumber()));
    ASSERT_NO_THROW(EXPECT_EQ(common::PhoneNumber{}, reader.readPhoneNumber()));
    ASSERT_NO_THROW(EXPECT_EQ(BTS_ID, reader.readBtsId()));
    ASSERT_NO_THROW(reader.checkEndOfMessage());
}

TEST_F(BtsPortTestSuite, shallSendSmsMessage)
{
    const common::PhoneNumber TO_NUMBER{123};
    const std::string SMS_TEXT{"Hello World!"};
    
    common::BinaryMessage msg;
    EXPECT_CALL(transportMock, sendMessage(_)).WillOnce([&msg](auto param) { msg = std::move(param); return true; });
    
    objectUnderTest.sendSms(TO_NUMBER, SMS_TEXT);
    
    common::IncomingMessage reader(msg);
    ASSERT_NO_THROW(EXPECT_EQ(common::MessageId::Sms, reader.readMessageId()));
    ASSERT_NO_THROW(EXPECT_EQ(PHONE_NUMBER, reader.readPhoneNumber()));
    ASSERT_NO_THROW(EXPECT_EQ(TO_NUMBER, reader.readPhoneNumber()));
    ASSERT_NO_THROW(EXPECT_EQ(SMS_TEXT, reader.readRemainingText()));
    ASSERT_NO_THROW(reader.checkEndOfMessage());
}

TEST_F(BtsPortTestSuite, shallReceiveSmsMessage)
{
    const common::PhoneNumber FROM_NUMBER{124};
    const std::string SMS_TEXT{"Test SMS message"};
    
    EXPECT_CALL(handlerMock, handleSms(FROM_NUMBER, SMS_TEXT));
    
    common::OutgoingMessage msg{common::MessageId::Sms, FROM_NUMBER, PHONE_NUMBER};
    msg.writeText(SMS_TEXT);
    messageCallback(msg.getMessage());
}

TEST_F(BtsPortTestSuite, shallHandleSmsTransportFailure)
{
    const common::PhoneNumber TO_NUMBER{123};
    const std::string SMS_TEXT{"Test message"};
    
    EXPECT_CALL(transportMock, sendMessage(_)).WillOnce(Return(false));
    
    ASSERT_NO_THROW(objectUnderTest.sendSms(TO_NUMBER, SMS_TEXT));
}

TEST_F(BtsPortTestSuite, shallRetryFailedSmsTransmission)
{
    const common::PhoneNumber TO_NUMBER{123};
    const std::string SMS_TEXT{"Retry test"};
    
    // First attempt fails, second succeeds
    EXPECT_CALL(transportMock, sendMessage(_))
        .WillOnce(Return(false))
        .WillOnce(Return(true));
    
    // First attempt
    objectUnderTest.sendSms(TO_NUMBER, SMS_TEXT);
    
    // Retry attempt
    objectUnderTest.sendSms(TO_NUMBER, SMS_TEXT);
}

TEST_F(BtsPortTestSuite, shallHandleInvalidSmsFormat)
{
    // Create malformed message (wrong message type but SMS handler called)
    common::OutgoingMessage wrongMsg{common::MessageId::CallRequest,
                                   common::PhoneNumber{125},
                                   PHONE_NUMBER};
    wrongMsg.writeText("This should not be handled as SMS");
    
    // Should not call handleSms for non-SMS messages
    EXPECT_CALL(handlerMock, handleSms(_, _)).Times(0);
    EXPECT_CALL(handlerMock, handleCallRequest(_)).Times(1);
    
    messageCallback(wrongMsg.getMessage());
}

}