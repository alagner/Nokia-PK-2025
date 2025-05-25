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
        ON_CALL(transportMock, sendMessage(_)).WillByDefault(Return(true));
        EXPECT_CALL(transportMock, sendMessage(_)).Times(AnyNumber());
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

TEST_F(BtsPortTestSuite, shallHandleSmsMessage)
{
    const std::string TEXT = "Hello from BTS";
    const common::PhoneNumber FROM{123};

    EXPECT_CALL(handlerMock, handleSms(FROM, TEXT));

    common::OutgoingMessage msg{common::MessageId::Sms, FROM, PHONE_NUMBER};
    msg.writeText(TEXT);

    messageCallback(msg.getMessage());
}


TEST_F(BtsPortTestSuite, shallHandleDisConnect)
{
    EXPECT_CALL(handlerMock, handleDisconnect());
    disconnectedCallback();
}

TEST_F(BtsPortTestSuite, shallHandleCallTalk)
{
    const common::PhoneNumber FROM{11};
    EXPECT_CALL(handlerMock, handleTalkMessage(FROM, "Hi!"));

    common::OutgoingMessage msg{common::MessageId::CallTalk, FROM, PHONE_NUMBER};
    msg.writeText("Hi!");
    messageCallback(msg.getMessage());
}

TEST_F(BtsPortTestSuite, shallHandleUnknownRecipientAfterCallTalk)
{
    const common::PhoneNumber PEER{22};
    objectUnderTest.sendTalkMessage(PEER, "abc");

    EXPECT_CALL(handlerMock, handleCallRecipientNotAvailable());

    common::OutgoingMessage msg{common::MessageId::UnknownRecipient, PHONE_NUMBER, common::PhoneNumber{}};
    msg.writeNumber<uint8_t>(0);
    messageCallback(msg.getMessage());
}

TEST_F(BtsPortTestSuite, shallHandleUnknownRecipientAfterCallRequest)
{
    const common::PhoneNumber PEER{33};
    objectUnderTest.sendCallRequest(PHONE_NUMBER, PEER);

    EXPECT_CALL(handlerMock, handleCallRecipientNotAvailable());

    common::OutgoingMessage msg{common::MessageId::UnknownRecipient, PHONE_NUMBER, common::PhoneNumber{}};
    msg.writeNumber<uint8_t>(0);
    messageCallback(msg.getMessage());
}

TEST_F(BtsPortTestSuite, shallSendTalkMessage)
{
    const common::PhoneNumber TO{124};
    const std::string TEXT = "Let's talk!";
    common::BinaryMessage sentMsg;

    EXPECT_CALL(transportMock, sendMessage(_))
        .WillOnce([&sentMsg](auto param) {
            sentMsg = std::move(param);
            return true;
        });

    objectUnderTest.sendTalkMessage(TO, TEXT);

    common::IncomingMessage reader(sentMsg);
    EXPECT_EQ(common::MessageId::CallTalk, reader.readMessageId());
    EXPECT_EQ(PHONE_NUMBER, reader.readPhoneNumber());
    EXPECT_EQ(TO, reader.readPhoneNumber());
    EXPECT_EQ(TEXT, reader.readRemainingText());
    EXPECT_NO_THROW(reader.checkEndOfMessage());
}

TEST_F(BtsPortTestSuite, shallHandleCallAccepted)
{
    const common::PhoneNumber caller{123};
    EXPECT_CALL(handlerMock, handleCallAccepted(caller));

    common::OutgoingMessage msg{common::MessageId::CallAccepted, caller, PHONE_NUMBER};
    messageCallback(msg.getMessage());
}

TEST_F(BtsPortTestSuite, shallHandleCallDropped)
{
    EXPECT_CALL(handlerMock, handleCallDropped());

    common::OutgoingMessage msg{common::MessageId::CallDropped,
                                common::PhoneNumber{123},
                                PHONE_NUMBER};
    messageCallback(msg.getMessage());
}

TEST_F(BtsPortTestSuite, shallHandleUnknownRecipientForCall)
{
    EXPECT_CALL(transportMock, sendMessage(_)).WillOnce(Return(true));

    objectUnderTest.sendCallRequest(PHONE_NUMBER, common::PhoneNumber{123});

    EXPECT_CALL(handlerMock, handleCallRecipientNotAvailable());

    common::OutgoingMessage msg{common::MessageId::UnknownRecipient,
                                PHONE_NUMBER,
                                common::PhoneNumber{}};
    messageCallback(msg.getMessage());
}

TEST_F(BtsPortTestSuite, shallHandleUnknownRecipientForSms)
{
    EXPECT_CALL(transportMock, sendMessage(_)).WillOnce(Return(true));

    objectUnderTest.sendSms(SmsEntity{PHONE_NUMBER.value, 123, "test message"});

    EXPECT_CALL(handlerMock, handleSmsDeliveryFailure(common::PhoneNumber{123}));

    common::OutgoingMessage msg{common::MessageId::UnknownRecipient,
                                PHONE_NUMBER,
                                common::PhoneNumber{123}};
    messageCallback(msg.getMessage());
}

TEST_F(BtsPortTestSuite, shallSendCallRequest)
{
    common::BinaryMessage msg;
    const common::PhoneNumber from = PHONE_NUMBER;
    const common::PhoneNumber to{123};
    EXPECT_CALL(transportMock, sendMessage(_)).WillOnce([&msg](auto param) {
        msg = std::move(param);
        return true;
    });
    objectUnderTest.sendCallRequest(from, to);
    common::IncomingMessage reader(msg);
    ASSERT_EQ(reader.readMessageId(), common::MessageId::CallRequest);
    ASSERT_EQ(reader.readPhoneNumber(), from);
    ASSERT_EQ(reader.readPhoneNumber(), to);
    ASSERT_NO_THROW(reader.checkEndOfMessage());
}

TEST_F(BtsPortTestSuite, shallSendCallDropped)
{
    common::BinaryMessage msg;
    const common::PhoneNumber from = PHONE_NUMBER;
    const common::PhoneNumber to{123};
    EXPECT_CALL(transportMock, sendMessage(_)).WillOnce([&msg](auto param) {
        msg = std::move(param);
        return true;
    });
    objectUnderTest.sendCallDropped(from, to);
    common::IncomingMessage reader(msg);
    ASSERT_EQ(reader.readMessageId(), common::MessageId::CallDropped);
    ASSERT_EQ(reader.readPhoneNumber(), from);
    ASSERT_EQ(reader.readPhoneNumber(), to);
    ASSERT_NO_THROW(reader.checkEndOfMessage());
}

}
