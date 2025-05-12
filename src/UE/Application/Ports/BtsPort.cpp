#include "BtsPort.hpp"
#include "Messages/IncomingMessage.hpp"
#include "Messages/OutgoingMessage.hpp"

    namespace ue
{

    BtsPort::BtsPort(common::ILogger &logger, common::ITransport &transport, common::PhoneNumber phoneNumber)
        : logger(logger, "[BTS-PORT]"),
        transport(transport),
        phoneNumber(phoneNumber)
    {}

    void BtsPort::start(IBtsEventsHandler &handler)
    {
        transport.registerMessageCallback([this](BinaryMessage msg) {handleMessage(msg);});
        transport.registerDisconnectedCallback([this](){handleDisconnected();});
        this->handler = &handler;
    }

    void BtsPort::stop()
    {
        transport.registerMessageCallback(nullptr);
        transport.registerDisconnectedCallback(nullptr);
        handler = nullptr;
    }

    void BtsPort::handleMessage(BinaryMessage msg)
    {
        try
        {
            common::IncomingMessage reader{msg};
            auto msgId = reader.readMessageId();
            auto from = reader.readPhoneNumber();
            auto to = reader.readPhoneNumber();

            switch (msgId)
            {
            case common::MessageId::Sib:
            {
                auto btsId = reader.readBtsId();
                handler->handleSib(btsId);
                break;
            }
            case common::MessageId::AttachResponse:
            {
                bool accept = reader.readNumber<std::uint8_t>() != 0u;
                if (accept)
                    handler->handleAttachAccept();
                else
                    handler->handleAttachReject();
                break;
            }
            case common::MessageId::Sms:
                {
                    std::string msgText = reader.readRemainingText();
                    handler->handleSmsReceive(from, msgText);
                    break;
                }
            case common::MessageId::UnknownRecipient:
                {
                    auto originalRecipient = reader.readPhoneNumber();
                    logger.logError("SMS sending failed - Unknown recipient: ", originalRecipient);
                    if (handler)
                        handler->handleSmsSent(originalRecipient, false);
                    break;
                }
            //TODO: add more cases here

            default:
                logger.logError("unknown message: ", msgId, ", from: ", from);

            }
        }
        catch (std::exception const& ex)
        {
            logger.logError("handleMessage error: ", ex.what());
        }
    }

    void BtsPort::handleDisconnected()
    {
        logger.logInfo("Transport Disconnected");
        if (handler)
        {
            handler->handleDisconnected();
        }
    }

    void BtsPort::sendAttachRequest(common::BtsId btsId)
    {
        logger.logDebug("sendAttachRequest: ", btsId);
        common::OutgoingMessage msg{common::MessageId::AttachRequest,
                                    phoneNumber,
                                    common::PhoneNumber{}};
        msg.writeBtsId(btsId);
        transport.sendMessage(msg.getMessage());
    }

    void BtsPort::sendSms(common::PhoneNumber to, const std::string& textMessage)
    {
        logger.logInfo("Sending SMS to: ", to);
        common::OutgoingMessage msg{
            common::MessageId::Sms,
            phoneNumber,
            to
        };
        msg.writeText(textMessage);
        transport.sendMessage(msg.getMessage());
    }

}
