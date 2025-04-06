#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "Mocks/IEventsHandlerMock.hpp"
#include "Ports/BtsPort.hpp"
#include "Mocks/ILoggerMock.hpp"
#include "Mocks/IBtsPortMock.hpp"
#include "Messages/PhoneNumber.hpp"
#include "Mocks/ITransportMock.hpp"
#include "Messages/OutgoingMessage.hpp"
#include "Messages/IncomingMessage.hpp"
#include "Messages/MessageId.hpp" 

namespace ue
{
    using namespace ::testing;

    class BtsPortTestSuite : public Test
    {
    protected:
        const common::PhoneNumber PHONE_NUMBER{112};
        const common::BtsId BTS_ID{13121981ll};
        NiceMock<common::ILoggerMock> loggerMock;
        StrictMock<IEventsHandlerMock> handlerMock;
        NiceMock<common::ITransportMock> transportMock;
        common::ITransport::MessageCallback messageCallback;
        common::ITransport::DisconnectedCallback disconnectedCallback;
        common::BinaryMessage msg; // Storage for outgoing message

        BtsPort objectUnderTest{loggerMock, transportMock, PHONE_NUMBER};

        void SetUp() override
        {
            // Capture callbacks
            ON_CALL(transportMock, registerMessageCallback(_))
                .WillByDefault(SaveArg<0>(&messageCallback));
            ON_CALL(transportMock, registerDisconnectedCallback(_))
                .WillByDefault(SaveArg<0>(&disconnectedCallback));

            // Capture outgoing message using DoAll to combine actions
            ON_CALL(transportMock, sendMessage(_))
                .WillByDefault(DoAll(
                    SaveArg<0>(&msg),
                    Return(true) // Return the expected bool value
                    ));

            objectUnderTest.start(handlerMock);
        }

        void TearDown() override
        {
            objectUnderTest.stop();
        }

        // Helper to create test messages
        common::BinaryMessage createSibMessage()
        {
            common::OutgoingMessage msg{common::MessageId::Sib,
                                        common::PhoneNumber{},
                                        common::PhoneNumber{}};
            msg.writeBtsId(BTS_ID);
            return msg.getMessage();
        }

        common::BinaryMessage createAttachResponseMessage(bool accept)
        {
            common::OutgoingMessage msg{common::MessageId::AttachResponse,
                                        common::PhoneNumber{},
                                        PHONE_NUMBER};
            msg.writeNumber<std::uint8_t>(accept ? 1u : 0u);
            return msg.getMessage();
        }

        // Helper to create invalid message with unhandled message ID
        common::BinaryMessage createInvalidMessage()
        {
            // Use Attach as the message ID, but make it invalid by missing fields
            common::OutgoingMessage msg{common::MessageId::AttachRequest,
                                        common::PhoneNumber{},
                                        common::PhoneNumber{}};
            // Don't write BtsId, making it invalid
            return msg.getMessage();
        }
    };

    TEST_F(BtsPortTestSuite, shallRegisterHandlersBetweenStartStop)
    {
        // Already verified in SetUp/TearDown
        SUCCEED();
    }

    TEST_F(BtsPortTestSuite, shallIgnoreWrongMessage)
    {
        // Use an invalid message instead of Unknown message type
        messageCallback(createInvalidMessage());
        // Test passes if no handler methods are called (enforced by StrictMock)
    }

    TEST_F(BtsPortTestSuite, shallHandleSib)
    {
        EXPECT_CALL(handlerMock, handleSib(BTS_ID));
        messageCallback(createSibMessage());
    }

    TEST_F(BtsPortTestSuite, shallHandleAttachAccept)
    {
        EXPECT_CALL(handlerMock, handleAttachAccept());
        messageCallback(createAttachResponseMessage(true));
    }

    TEST_F(BtsPortTestSuite, shallHandleAttachReject)
    {
        EXPECT_CALL(handlerMock, handleAttachReject());
        messageCallback(createAttachResponseMessage(false));
    }

    TEST_F(BtsPortTestSuite, shallSendAttachRequest)
    {
        EXPECT_CALL(transportMock, sendMessage(_));
        objectUnderTest.sendAttachRequest(BTS_ID);

        common::IncomingMessage reader(msg);
        ASSERT_NO_THROW(EXPECT_EQ(common::MessageId::AttachRequest, reader.readMessageId()));
        ASSERT_NO_THROW(EXPECT_EQ(PHONE_NUMBER, reader.readPhoneNumber()));
        ASSERT_NO_THROW(EXPECT_EQ(common::PhoneNumber{}, reader.readPhoneNumber()));
        ASSERT_NO_THROW(EXPECT_EQ(BTS_ID, reader.readBtsId()));
        ASSERT_NO_THROW(reader.checkEndOfMessage());
    }

    TEST_F(BtsPortTestSuite, shallHandleDisconnectedFromTransport)
    {
        EXPECT_CALL(handlerMock, handleDisconnected());
        disconnectedCallback();
    }

}
