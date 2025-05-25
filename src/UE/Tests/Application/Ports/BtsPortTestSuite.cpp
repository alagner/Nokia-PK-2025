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

#pragma region Base Functionality Tests
TEST_F(BtsPortTestSuite, shallRegisterHandlersBetweenStartStop)
{
}

TEST_F(BtsPortTestSuite, shallIgnoreWrongMessage)
{
    common::OutgoingMessage wrongMsg{};
    wrongMsg.writeBtsId(BTS_ID);
    messageCallback(wrongMsg.getMessage());
}
#pragma endregion

#pragma region Attach/Detach Tests
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
#pragma endregion

#pragma region SMS Tests
TEST_F(BtsPortTestSuite, shallSendSmsMessage)
{
    const common::PhoneNumber TO_NUMBER{123};
    const std::string SMS_TEXT{"Hello1"};
    
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
    const std::string SMS_TEXT{"Hello1"};
    
    EXPECT_CALL(handlerMock, handleSms(FROM_NUMBER, SMS_TEXT));
    
    common::OutgoingMessage msg{common::MessageId::Sms, FROM_NUMBER, PHONE_NUMBER};
    msg.writeText(SMS_TEXT);
    messageCallback(msg.getMessage());
}

TEST_F(BtsPortTestSuite, shallHandleSmsTransportFailure)
{
    const common::PhoneNumber TO_NUMBER{123};
    const std::string SMS_TEXT{"Hello1"};
    
    EXPECT_CALL(transportMock, sendMessage(_)).WillOnce(Return(false));
    
    ASSERT_NO_THROW(objectUnderTest.sendSms(TO_NUMBER, SMS_TEXT));
}

TEST_F(BtsPortTestSuite, shallRetryFailedSmsTransmission)
{
    const common::PhoneNumber TO_NUMBER{123};
    const std::string SMS_TEXT{"Hello1"};
    
    EXPECT_CALL(transportMock, sendMessage(_))
        .WillOnce(Return(false))
        .WillOnce(Return(true));
    
    objectUnderTest.sendSms(TO_NUMBER, SMS_TEXT);
    objectUnderTest.sendSms(TO_NUMBER, SMS_TEXT);
}

TEST_F(BtsPortTestSuite, shallHandleInvalidSmsFormat)
{
    // (wrong message type but SMS handler called)
    common::OutgoingMessage wrongMsg{common::MessageId::CallRequest,
                                   common::PhoneNumber{125},
                                   PHONE_NUMBER};
    wrongMsg.writeText("Hello1");

    EXPECT_CALL(handlerMock, handleSms(_, _)).Times(0);
    EXPECT_CALL(handlerMock, handleCallRequest(_)).Times(1);
    
    messageCallback(wrongMsg.getMessage());
}

TEST_F(BtsPortTestSuite, shallHandleEmptySmsText)
{
    const common::PhoneNumber TO_NUMBER{123};
    const std::string EMPTY_SMS_TEXT{""};
    
    common::BinaryMessage msg;
    EXPECT_CALL(transportMock, sendMessage(_)).WillOnce([&msg](auto param) { msg = std::move(param); return true; });
    
    objectUnderTest.sendSms(TO_NUMBER, EMPTY_SMS_TEXT);
    
    common::IncomingMessage reader(msg);
    ASSERT_NO_THROW(EXPECT_EQ(common::MessageId::Sms, reader.readMessageId()));
    ASSERT_NO_THROW(EXPECT_EQ(PHONE_NUMBER, reader.readPhoneNumber()));
    ASSERT_NO_THROW(EXPECT_EQ(TO_NUMBER, reader.readPhoneNumber()));
    ASSERT_NO_THROW(EXPECT_EQ(EMPTY_SMS_TEXT, reader.readRemainingText()));
    ASSERT_NO_THROW(reader.checkEndOfMessage());
    
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
    
    const common::PhoneNumber FROM_NUMBER{125};
    EXPECT_CALL(handlerMock, handleSms(FROM_NUMBER, LONG_SMS_TEXT));
    
    common::OutgoingMessage incomingMsg{common::MessageId::Sms, FROM_NUMBER, PHONE_NUMBER};
    incomingMsg.writeText(LONG_SMS_TEXT);
    messageCallback(incomingMsg.getMessage());
}

TEST_F(BtsPortTestSuite, shallHandleInvalidPhoneNumber)
{
    const std::string SMS_TEXT{"Hello1"};
    
    const common::PhoneNumber INVALID_FROM_NUMBER{0};
    EXPECT_CALL(handlerMock, handleSms(INVALID_FROM_NUMBER, SMS_TEXT));
    
    common::OutgoingMessage incomingMsg{common::MessageId::Sms, INVALID_FROM_NUMBER, PHONE_NUMBER};
    incomingMsg.writeText(SMS_TEXT);
    messageCallback(incomingMsg.getMessage());
    
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
    
    common::OutgoingMessage msg1{common::MessageId::Sms, FROM_NUMBER1, PHONE_NUMBER};
    msg1.writeText(SMS_TEXT1);
    messageCallback(msg1.getMessage());
    
    common::OutgoingMessage msg2{common::MessageId::Sms, FROM_NUMBER2, PHONE_NUMBER};
    msg2.writeText(SMS_TEXT2);
    messageCallback(msg2.getMessage());
    
    common::OutgoingMessage msg3{common::MessageId::Sms, FROM_NUMBER3, PHONE_NUMBER};
    msg3.writeText(SMS_TEXT3);
    messageCallback(msg3.getMessage());
    
    const common::PhoneNumber TO_NUMBER1{129};
    const common::PhoneNumber TO_NUMBER2{130};
    const std::string OUTGOING_TEXT1{"Hello21"};
    const std::string OUTGOING_TEXT2{"Hello22"};
    
    common::BinaryMessage outMsg1, outMsg2;
    EXPECT_CALL(transportMock, sendMessage(_))
        .WillOnce([&outMsg1](auto param) { outMsg1 = std::move(param); return true; })
        .WillOnce([&outMsg2](auto param) { outMsg2 = std::move(param); return true; });
    
    objectUnderTest.sendSms(TO_NUMBER1, OUTGOING_TEXT1);
    objectUnderTest.sendSms(TO_NUMBER2, OUTGOING_TEXT2);
    
    common::IncomingMessage reader1(outMsg1);
    ASSERT_NO_THROW(EXPECT_EQ(common::MessageId::Sms, reader1.readMessageId()));
    ASSERT_NO_THROW(EXPECT_EQ(PHONE_NUMBER, reader1.readPhoneNumber()));
    ASSERT_NO_THROW(EXPECT_EQ(TO_NUMBER1, reader1.readPhoneNumber()));
    ASSERT_NO_THROW(EXPECT_EQ(OUTGOING_TEXT1, reader1.readRemainingText()));
    ASSERT_NO_THROW(reader1.checkEndOfMessage());
    
    common::IncomingMessage reader2(outMsg2);
    ASSERT_NO_THROW(EXPECT_EQ(common::MessageId::Sms, reader2.readMessageId()));
    ASSERT_NO_THROW(EXPECT_EQ(PHONE_NUMBER, reader2.readPhoneNumber()));
    ASSERT_NO_THROW(EXPECT_EQ(TO_NUMBER2, reader2.readPhoneNumber()));
    ASSERT_NO_THROW(EXPECT_EQ(OUTGOING_TEXT2, reader2.readRemainingText()));
    ASSERT_NO_THROW(reader2.checkEndOfMessage());
}
#pragma endregion

#pragma region Call Control Tests
TEST_F(BtsPortTestSuite, shallReceiveCallRequest)
{
    const common::PhoneNumber FROM_NUMBER{124};
    
    EXPECT_CALL(handlerMock, handleCallRequest(FROM_NUMBER));
    
    common::OutgoingMessage msg{common::MessageId::CallRequest, FROM_NUMBER, PHONE_NUMBER};
    messageCallback(msg.getMessage());
}

TEST_F(BtsPortTestSuite, shallSendCallAccept)
{
    const common::PhoneNumber TO_NUMBER{123};
    
    common::BinaryMessage msg;
    EXPECT_CALL(transportMock, sendMessage(_)).WillOnce([&msg](auto param) { msg = std::move(param); return true; });
    
    objectUnderTest.sendCallAccept(TO_NUMBER);
    
    common::IncomingMessage reader(msg);
    ASSERT_NO_THROW(EXPECT_EQ(common::MessageId::CallAccepted, reader.readMessageId()));
    ASSERT_NO_THROW(EXPECT_EQ(PHONE_NUMBER, reader.readPhoneNumber()));
    ASSERT_NO_THROW(EXPECT_EQ(TO_NUMBER, reader.readPhoneNumber()));
    ASSERT_NO_THROW(reader.checkEndOfMessage());
}

TEST_F(BtsPortTestSuite, shallSendCallDropped)
{
    const common::PhoneNumber TO_NUMBER{123};
    
    common::BinaryMessage msg;
    EXPECT_CALL(transportMock, sendMessage(_)).WillOnce([&msg](auto param) { msg = std::move(param); return true; });
    
    objectUnderTest.sendCallDropped(TO_NUMBER);
    
    common::IncomingMessage reader(msg);
    ASSERT_NO_THROW(EXPECT_EQ(common::MessageId::CallDropped, reader.readMessageId()));
    ASSERT_NO_THROW(EXPECT_EQ(PHONE_NUMBER, reader.readPhoneNumber()));
    ASSERT_NO_THROW(EXPECT_EQ(TO_NUMBER, reader.readPhoneNumber()));
    ASSERT_NO_THROW(reader.checkEndOfMessage());
}

TEST_F(BtsPortTestSuite, shallSendCallRequest)
{
    const common::PhoneNumber TO_NUMBER{123};
    
    common::BinaryMessage msg;
    EXPECT_CALL(transportMock, sendMessage(_)).WillOnce([&msg](auto param) { msg = std::move(param); return true; });
    
    objectUnderTest.sendCallRequest(TO_NUMBER);
    
    common::IncomingMessage reader(msg);
    ASSERT_NO_THROW(EXPECT_EQ(common::MessageId::CallRequest, reader.readMessageId()));
    ASSERT_NO_THROW(EXPECT_EQ(PHONE_NUMBER, reader.readPhoneNumber()));
    ASSERT_NO_THROW(EXPECT_EQ(TO_NUMBER, reader.readPhoneNumber()));
    ASSERT_NO_THROW(reader.checkEndOfMessage());
}

TEST_F(BtsPortTestSuite, shallReceiveCallAccepted)
{
    const common::PhoneNumber FROM_NUMBER{124};
    
    EXPECT_CALL(handlerMock, handleCallAccepted(FROM_NUMBER));
    
    common::OutgoingMessage msg{common::MessageId::CallAccepted, FROM_NUMBER, PHONE_NUMBER};
    messageCallback(msg.getMessage());
}

TEST_F(BtsPortTestSuite, shallReceiveCallDropped)
{
    const common::PhoneNumber FROM_NUMBER{124};
    
    EXPECT_CALL(handlerMock, handleCallDropped(FROM_NUMBER));
    
    common::OutgoingMessage msg{common::MessageId::CallDropped, FROM_NUMBER, PHONE_NUMBER};
    messageCallback(msg.getMessage());
}
#pragma endregion

#pragma region Call Talk Tests
TEST_F(BtsPortTestSuite, shallHandleCallTalkSending)
{
    const common::PhoneNumber TO_NUMBER{123};
    const std::string CALL_TEXT{"Hello1"};
    
    common::BinaryMessage msg;
    EXPECT_CALL(transportMock, sendMessage(_)).WillOnce([&msg](auto param) { msg = std::move(param); return true; });
    
    objectUnderTest.sendCallTalk(TO_NUMBER, CALL_TEXT);
    
    common::IncomingMessage reader(msg);
    ASSERT_NO_THROW(EXPECT_EQ(common::MessageId::CallTalk, reader.readMessageId()));
    ASSERT_NO_THROW(EXPECT_EQ(PHONE_NUMBER, reader.readPhoneNumber()));
    ASSERT_NO_THROW(EXPECT_EQ(TO_NUMBER, reader.readPhoneNumber()));
    ASSERT_NO_THROW(EXPECT_EQ(CALL_TEXT, reader.readRemainingText()));
    ASSERT_NO_THROW(reader.checkEndOfMessage());
}

TEST_F(BtsPortTestSuite, shallHandleCallTalkReceiving)
{
    const common::PhoneNumber FROM_NUMBER{124};
    const std::string CALL_TEXT{"Hello1"};
    
    EXPECT_CALL(handlerMock, handleCallTalk(FROM_NUMBER, CALL_TEXT));
    
    common::OutgoingMessage msg{common::MessageId::CallTalk, FROM_NUMBER, PHONE_NUMBER};
    msg.writeText(CALL_TEXT);
    messageCallback(msg.getMessage());
}
#pragma endregion

#pragma region Error Handling Tests
TEST_F(BtsPortTestSuite, shallHandleCallRequestFailure)
{
    const common::PhoneNumber TO_NUMBER{123};
    
    EXPECT_CALL(transportMock, sendMessage(_)).WillOnce(Return(false));
    
    ASSERT_NO_THROW(objectUnderTest.sendCallRequest(TO_NUMBER));
}

TEST_F(BtsPortTestSuite, shallHandleCallAcceptFailure)
{
    const common::PhoneNumber TO_NUMBER{123};
    
    EXPECT_CALL(transportMock, sendMessage(_)).WillOnce(Return(false));
    
    ASSERT_NO_THROW(objectUnderTest.sendCallAccept(TO_NUMBER));
}

TEST_F(BtsPortTestSuite, shallHandleCallTalkFailure)
{
    const common::PhoneNumber TO_NUMBER{123};
    const std::string CALL_TEXT{"Hello1"};
    
    EXPECT_CALL(transportMock, sendMessage(_)).WillOnce(Return(false));
    
    ASSERT_NO_THROW(objectUnderTest.sendCallTalk(TO_NUMBER, CALL_TEXT));
}

TEST_F(BtsPortTestSuite, shallHandleCallDropFailure)
{
    const common::PhoneNumber TO_NUMBER{123};
    
    EXPECT_CALL(transportMock, sendMessage(_)).WillOnce(Return(false));
    
    ASSERT_NO_THROW(objectUnderTest.sendCallDropped(TO_NUMBER));
}
#pragma endregion

#pragma region Multiple Message Tests
TEST_F(BtsPortTestSuite, shallHandleMultipleCallMessages)
{
    
    const common::PhoneNumber PEER_NUMBER{125};
    const std::string CALL_TEXT{"Hello1"};
    
    common::BinaryMessage reqMsg, acceptMsg, talkMsg, dropMsg;
    EXPECT_CALL(transportMock, sendMessage(_))
        .WillOnce([&reqMsg](auto param) { reqMsg = std::move(param); return true; })
        .WillOnce([&acceptMsg](auto param) { acceptMsg = std::move(param); return true; })
        .WillOnce([&talkMsg](auto param) { talkMsg = std::move(param); return true; })
        .WillOnce([&dropMsg](auto param) { dropMsg = std::move(param); return true; });
    
    objectUnderTest.sendCallRequest(PEER_NUMBER);
    objectUnderTest.sendCallAccept(PEER_NUMBER);
    objectUnderTest.sendCallTalk(PEER_NUMBER, CALL_TEXT);
    objectUnderTest.sendCallDropped(PEER_NUMBER);
    
    common::IncomingMessage reqReader(reqMsg);
    ASSERT_NO_THROW(EXPECT_EQ(common::MessageId::CallRequest, reqReader.readMessageId()));
    ASSERT_NO_THROW(EXPECT_EQ(PHONE_NUMBER, reqReader.readPhoneNumber()));
    ASSERT_NO_THROW(EXPECT_EQ(PEER_NUMBER, reqReader.readPhoneNumber()));
    ASSERT_NO_THROW(reqReader.checkEndOfMessage());

    common::IncomingMessage acceptReader(acceptMsg);
    ASSERT_NO_THROW(EXPECT_EQ(common::MessageId::CallAccepted, acceptReader.readMessageId()));
    ASSERT_NO_THROW(EXPECT_EQ(PHONE_NUMBER, acceptReader.readPhoneNumber()));
    ASSERT_NO_THROW(EXPECT_EQ(PEER_NUMBER, acceptReader.readPhoneNumber()));
    ASSERT_NO_THROW(acceptReader.checkEndOfMessage());
    
    common::IncomingMessage talkReader(talkMsg);
    ASSERT_NO_THROW(EXPECT_EQ(common::MessageId::CallTalk, talkReader.readMessageId()));
    ASSERT_NO_THROW(EXPECT_EQ(PHONE_NUMBER, talkReader.readPhoneNumber()));
    ASSERT_NO_THROW(EXPECT_EQ(PEER_NUMBER, talkReader.readPhoneNumber()));
    ASSERT_NO_THROW(EXPECT_EQ(CALL_TEXT, talkReader.readRemainingText()));
    ASSERT_NO_THROW(talkReader.checkEndOfMessage());
    
    common::IncomingMessage dropReader(dropMsg);
    ASSERT_NO_THROW(EXPECT_EQ(common::MessageId::CallDropped, dropReader.readMessageId()));
    ASSERT_NO_THROW(EXPECT_EQ(PHONE_NUMBER, dropReader.readPhoneNumber()));
    ASSERT_NO_THROW(EXPECT_EQ(PEER_NUMBER, dropReader.readPhoneNumber()));
    ASSERT_NO_THROW(dropReader.checkEndOfMessage());
}
#pragma endregion

#pragma region Edge Cases Tests
TEST_F(BtsPortTestSuite, shallHandleLongCallTalkMessage)
{
    const common::PhoneNumber TO_NUMBER{123};
    const std::string LONG_TALK_TEXT(500, 'T'); 
    
    common::BinaryMessage msg;
    EXPECT_CALL(transportMock, sendMessage(_)).WillOnce([&msg](auto param) { msg = std::move(param); return true; });
    
    objectUnderTest.sendCallTalk(TO_NUMBER, LONG_TALK_TEXT);
    
    common::IncomingMessage reader(msg);
    ASSERT_NO_THROW(EXPECT_EQ(common::MessageId::CallTalk, reader.readMessageId()));
    ASSERT_NO_THROW(EXPECT_EQ(PHONE_NUMBER, reader.readPhoneNumber()));
    ASSERT_NO_THROW(EXPECT_EQ(TO_NUMBER, reader.readPhoneNumber()));
    ASSERT_NO_THROW(EXPECT_EQ(LONG_TALK_TEXT, reader.readRemainingText()));
    ASSERT_NO_THROW(reader.checkEndOfMessage());
}

TEST_F(BtsPortTestSuite, shallHandleEmptyCallTalkMessage)
{
    const common::PhoneNumber TO_NUMBER{123};
    const std::string EMPTY_TALK_TEXT{""};
    
    common::BinaryMessage msg;
    EXPECT_CALL(transportMock, sendMessage(_)).WillOnce([&msg](auto param) { msg = std::move(param); return true; });
    
    objectUnderTest.sendCallTalk(TO_NUMBER, EMPTY_TALK_TEXT);
    
    common::IncomingMessage reader(msg);
    ASSERT_NO_THROW(EXPECT_EQ(common::MessageId::CallTalk, reader.readMessageId()));
    ASSERT_NO_THROW(EXPECT_EQ(PHONE_NUMBER, reader.readPhoneNumber()));
    ASSERT_NO_THROW(EXPECT_EQ(TO_NUMBER, reader.readPhoneNumber()));
    ASSERT_NO_THROW(EXPECT_EQ(EMPTY_TALK_TEXT, reader.readRemainingText()));
    ASSERT_NO_THROW(reader.checkEndOfMessage());
}
#pragma endregion

}