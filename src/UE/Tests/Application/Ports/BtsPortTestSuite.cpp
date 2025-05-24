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

TEST_F(BtsPortTestSuite, shallHandleEmptySmsText)
{
    const common::PhoneNumber TO_NUMBER{123};
    const std::string EMPTY_SMS_TEXT{""};
    
    // Test sending SMS with empty text
    common::BinaryMessage msg;
    EXPECT_CALL(transportMock, sendMessage(_)).WillOnce([&msg](auto param) { msg = std::move(param); return true; });
    
    objectUnderTest.sendSms(TO_NUMBER, EMPTY_SMS_TEXT);
    
    common::IncomingMessage reader(msg);
    ASSERT_NO_THROW(EXPECT_EQ(common::MessageId::Sms, reader.readMessageId()));
    ASSERT_NO_THROW(EXPECT_EQ(PHONE_NUMBER, reader.readPhoneNumber()));
    ASSERT_NO_THROW(EXPECT_EQ(TO_NUMBER, reader.readPhoneNumber()));
    ASSERT_NO_THROW(EXPECT_EQ(EMPTY_SMS_TEXT, reader.readRemainingText()));
    ASSERT_NO_THROW(reader.checkEndOfMessage());
    
    // Test receiving SMS with empty text
    const common::PhoneNumber FROM_NUMBER{124};
    EXPECT_CALL(handlerMock, handleSms(FROM_NUMBER, EMPTY_SMS_TEXT));
    
    common::OutgoingMessage incomingMsg{common::MessageId::Sms, FROM_NUMBER, PHONE_NUMBER};
    incomingMsg.writeText(EMPTY_SMS_TEXT);
    messageCallback(incomingMsg.getMessage());
}

TEST_F(BtsPortTestSuite, shallHandleLongSmsText)
{
    const common::PhoneNumber TO_NUMBER{123};
    const std::string LONG_SMS_TEXT(161, 'A');
    
    common::BinaryMessage msg;
    EXPECT_CALL(transportMock, sendMessage(_)).WillOnce([&msg](auto param) { msg = std::move(param); return true; });
    
    objectUnderTest.sendSms(TO_NUMBER, LONG_SMS_TEXT);
    
    common::IncomingMessage reader(msg);
    ASSERT_NO_THROW(EXPECT_EQ(common::MessageId::Sms, reader.readMessageId()));
    ASSERT_NO_THROW(EXPECT_EQ(PHONE_NUMBER, reader.readPhoneNumber()));
    ASSERT_NO_THROW(EXPECT_EQ(TO_NUMBER, reader.readPhoneNumber()));
    ASSERT_NO_THROW(EXPECT_EQ(LONG_SMS_TEXT, reader.readRemainingText()));
    ASSERT_NO_THROW(reader.checkEndOfMessage());
    
    // Test receiving long SMS
    const common::PhoneNumber FROM_NUMBER{125};
    EXPECT_CALL(handlerMock, handleSms(FROM_NUMBER, LONG_SMS_TEXT));
    
    common::OutgoingMessage incomingMsg{common::MessageId::Sms, FROM_NUMBER, PHONE_NUMBER};
    incomingMsg.writeText(LONG_SMS_TEXT);
    messageCallback(incomingMsg.getMessage());
}

TEST_F(BtsPortTestSuite, shallHandleInvalidPhoneNumber)
{
    const std::string SMS_TEXT{"Hello1"};
    
    // Test with invalid/empty phone number as sender
    const common::PhoneNumber INVALID_FROM_NUMBER{0};
    EXPECT_CALL(handlerMock, handleSms(INVALID_FROM_NUMBER, SMS_TEXT));
    
    common::OutgoingMessage incomingMsg{common::MessageId::Sms, INVALID_FROM_NUMBER, PHONE_NUMBER};
    incomingMsg.writeText(SMS_TEXT);
    messageCallback(incomingMsg.getMessage());
    
    // Test sending SMS to invalid phone number
    const common::PhoneNumber INVALID_TO_NUMBER{0};
    common::BinaryMessage msg;
    EXPECT_CALL(transportMock, sendMessage(_)).WillOnce([&msg](auto param) { msg = std::move(param); return true; });
    
    objectUnderTest.sendSms(INVALID_TO_NUMBER, SMS_TEXT);
    
    common::IncomingMessage reader(msg);
    ASSERT_NO_THROW(EXPECT_EQ(common::MessageId::Sms, reader.readMessageId()));
    ASSERT_NO_THROW(EXPECT_EQ(PHONE_NUMBER, reader.readPhoneNumber()));
    ASSERT_NO_THROW(EXPECT_EQ(INVALID_TO_NUMBER, reader.readPhoneNumber()));
    ASSERT_NO_THROW(EXPECT_EQ(SMS_TEXT, reader.readRemainingText()));
    ASSERT_NO_THROW(reader.checkEndOfMessage());
    
    // Test with large phone number
    const common::PhoneNumber LARGE_NUMBER{255};
    EXPECT_CALL(handlerMock, handleSms(LARGE_NUMBER, SMS_TEXT));
    
    common::OutgoingMessage largeNumberMsg{common::MessageId::Sms, LARGE_NUMBER, PHONE_NUMBER};
    largeNumberMsg.writeText(SMS_TEXT);
    messageCallback(largeNumberMsg.getMessage());
}

TEST_F(BtsPortTestSuite, shallHandleMultipleSmsMessages)
{
    const common::PhoneNumber FROM_NUMBER1{126};
    const common::PhoneNumber FROM_NUMBER2{127};
    const common::PhoneNumber FROM_NUMBER3{128};
    const std::string SMS_TEXT1{"Hello1"};
    const std::string SMS_TEXT2{"Hello2"};
    const std::string SMS_TEXT3{"Hello3"};
    
    EXPECT_CALL(handlerMock, handleSms(FROM_NUMBER1, SMS_TEXT1));
    EXPECT_CALL(handlerMock, handleSms(FROM_NUMBER2, SMS_TEXT2));
    EXPECT_CALL(handlerMock, handleSms(FROM_NUMBER3, SMS_TEXT3));
    
    // Send first SMS
    common::OutgoingMessage msg1{common::MessageId::Sms, FROM_NUMBER1, PHONE_NUMBER};
    msg1.writeText(SMS_TEXT1);
    messageCallback(msg1.getMessage());
    
    // Send second SMS
    common::OutgoingMessage msg2{common::MessageId::Sms, FROM_NUMBER2, PHONE_NUMBER};
    msg2.writeText(SMS_TEXT2);
    messageCallback(msg2.getMessage());
    
    // Send third SMS
    common::OutgoingMessage msg3{common::MessageId::Sms, FROM_NUMBER3, PHONE_NUMBER};
    msg3.writeText(SMS_TEXT3);
    messageCallback(msg3.getMessage());
    
    // Test sending multiple SMS messages in sequence
    const common::PhoneNumber TO_NUMBER1{129};
    const common::PhoneNumber TO_NUMBER2{130};
    const std::string OUTGOING_TEXT1{"Outgoing first"};
    const std::string OUTGOING_TEXT2{"Outgoing second"};
    
    common::BinaryMessage outMsg1, outMsg2;
    EXPECT_CALL(transportMock, sendMessage(_))
        .WillOnce([&outMsg1](auto param) { outMsg1 = std::move(param); return true; })
        .WillOnce([&outMsg2](auto param) { outMsg2 = std::move(param); return true; });
    
    objectUnderTest.sendSms(TO_NUMBER1, OUTGOING_TEXT1);
    objectUnderTest.sendSms(TO_NUMBER2, OUTGOING_TEXT2);
    
    // Verify first outgoing message
    common::IncomingMessage reader1(outMsg1);
    ASSERT_NO_THROW(EXPECT_EQ(common::MessageId::Sms, reader1.readMessageId()));
    ASSERT_NO_THROW(EXPECT_EQ(PHONE_NUMBER, reader1.readPhoneNumber()));
    ASSERT_NO_THROW(EXPECT_EQ(TO_NUMBER1, reader1.readPhoneNumber()));
    ASSERT_NO_THROW(EXPECT_EQ(OUTGOING_TEXT1, reader1.readRemainingText()));
    ASSERT_NO_THROW(reader1.checkEndOfMessage());
    
    // Verify second outgoing message
    common::IncomingMessage reader2(outMsg2);
    ASSERT_NO_THROW(EXPECT_EQ(common::MessageId::Sms, reader2.readMessageId()));
    ASSERT_NO_THROW(EXPECT_EQ(PHONE_NUMBER, reader2.readPhoneNumber()));
    ASSERT_NO_THROW(EXPECT_EQ(TO_NUMBER2, reader2.readPhoneNumber()));
    ASSERT_NO_THROW(EXPECT_EQ(OUTGOING_TEXT2, reader2.readRemainingText()));
    ASSERT_NO_THROW(reader2.checkEndOfMessage());
}

}