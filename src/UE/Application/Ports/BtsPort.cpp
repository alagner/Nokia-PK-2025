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
    transport.registerDisconnectedCallback([this]() {handleDisconnect();});
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

        logger.logDebug("Received message type: ", msgId, ", from: ", from, ", to: ", to);

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
            auto text = reader.readRemainingText();
            handler->handleSms(from, text);
            break;
        }
        case common::MessageId::CallRequest:
        {
            logger.logInfo("Received CallRequest from: ", from);
            handler->handleCallRequest(from);
            break;
        }
        case common::MessageId::CallAccepted:
        {
            logger.logInfo("Received CallAccepted from: ", from);
            handler->handleCallAccepted(from);
            break;
        }
        case common::MessageId::CallDropped:
        {
            logger.logInfo("Received CallDropped from: ", from);
            handler->handleCallDropped(from);
            break;
        }
        case common::MessageId::CallTalk:
        {
            auto text = reader.readRemainingText();
            logger.logInfo("Received CallTalk from: ", from, ", message: ", text);
            handler->handleCallTalk(from, text);
            break;
        }
        case common::MessageId::UnknownRecipient:
        {
            logger.logInfo("Received UnknownRecipient message, to: ", to);
            if (to == phoneNumber) {
                logger.logInfo("We were the sender, treating as CallDropped");
                handler->handleCallDropped(from);
            }
            break;
        }
        default:
            logger.logError("unknown message: ", msgId, ", from: ", from);
        }
    }
    catch (std::exception const& ex)
    {
        logger.logError("handleMessage error: ", ex.what());
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

void BtsPort::sendCallAccept(common::PhoneNumber to)
{
    logger.logDebug("sendCallAccept to: ", to);
    common::OutgoingMessage msg{common::MessageId::CallAccepted,
                                phoneNumber,
                                to};
    transport.sendMessage(msg.getMessage());
}

void BtsPort::sendCallDropped(common::PhoneNumber to)
{
    logger.logDebug("sendCallDropped to: ", to);
    common::OutgoingMessage msg{common::MessageId::CallDropped,
                                phoneNumber,
                                to};
    transport.sendMessage(msg.getMessage());
}

void BtsPort::sendCallTalk(common::PhoneNumber to, std::string message)
{
    logger.logDebug("sendCallTalk to: ", to, ", message: ", message);
    common::OutgoingMessage msg{common::MessageId::CallTalk,
                                phoneNumber,
                                to};
    msg.writeText(message);
    transport.sendMessage(msg.getMessage());
}

void BtsPort::sendCallRequest(common::PhoneNumber to)
{
    logger.logDebug("sendCallRequest to: ", to);
    common::OutgoingMessage msg{common::MessageId::CallRequest,
                                phoneNumber,
                                to};
    transport.sendMessage(msg.getMessage());
}

void BtsPort::sendSms(common::PhoneNumber to, const std::string& text)
{
    logger.logDebug("sendSms to: ", to, ", text: ", text);
    common::OutgoingMessage msg{common::MessageId::Sms,
                                phoneNumber,
                                to};
    msg.writeText(text);
    transport.sendMessage(msg.getMessage());
}

void BtsPort::handleDisconnect() {
    if(handler) {
        handler->handleDisconnect();
    }
}
}
