#include "BtsPort.hpp"
#include "Messages/IncomingMessage.hpp"
#include "Messages/OutgoingMessage.hpp"
#include "Utils/todo.h"

namespace ue
{

BtsPort::BtsPort(common::ILogger &logger, common::ITransport &transport, common::PhoneNumber phoneNumber)
    : logger(logger, "[BTS-PORT]"), transport(transport), phoneNumber(phoneNumber)
{}

void BtsPort::start(IBtsEventsHandler &handler)
{
    transport.registerMessageCallback([this](BinaryMessage msg) { handleMessage(msg); });
    transport.registerDisconnectedCallback([this]() { handleDisconnected(); });

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
        common::IncomingMessage reader{ msg };
        auto msgId = reader.readMessageId();
        auto from = reader.readPhoneNumber();
        auto to = reader.readPhoneNumber();

        switch (msgId)
        {
        case common::MessageId::Sib: {
            auto btsId = reader.readBtsId();
            handler->handleSib(btsId);
            break;
        }
        case common::MessageId::AttachResponse: {
            bool accept = reader.readNumber<std::uint8_t>() != 0u;
            if (accept)
                handler->handleAttachAccept();
            else
                handler->handleAttachReject();
            break;
        }
        case common::MessageId::Sms: {
            handler->handleMessageReceive(from, reader.readRemainingText());
            break;
        }
        case common::MessageId::CallRequest: {
            handler->handleCallRequest(from);
            break;
        }
        TODO(case CallDropped, CallTalk, CallAccepted, UnknownRecipient)
        default:
            logger.logError("unknow message: ", msgId, ", from: ", from);
        }
    }
    catch (std::exception const &ex)
    {
        logger.logError("handleMessage error: ", ex.what());
    }
}

void BtsPort::sendAttachRequest(common::BtsId btsId)
{
    logger.logDebug("sendAttachRequest: ", btsId);
    common::OutgoingMessage msg{ common::MessageId::AttachRequest, phoneNumber, common::PhoneNumber{} };
    msg.writeBtsId(btsId);
    transport.sendMessage(msg.getMessage());
}

void BtsPort::handleDisconnected()
{
    if (handler)
        handler->handleDisconnected();
}

void BtsPort::sendMessage(common::PhoneNumber to, const std::string &text)
{
    logger.logInfo("Sending SMS to: ", to);
    common::OutgoingMessage msg{ common::MessageId::Sms, phoneNumber, to };
    msg.writeText(text);
    transport.sendMessage(msg.getMessage());
}

void BtsPort::sendCallRequest(common::PhoneNumber to){
    logger.logInfo("sending Call Request to: ", to);

    common::OutgoingMessage msg{common::MessageId::CallRequest, phoneNumber, to};
    transport.sendMessage(msg.getMessage());
}

void BtsPort::sendCallDropped(common::PhoneNumber to)
{
    logger.logInfo("Sending CallDropped to: ", to);
    common::OutgoingMessage msg{ common::MessageId::CallDropped, phoneNumber, to };
    transport.sendMessage(msg.getMessage());
}

void BtsPort::sendCallTalk(common::PhoneNumber to, const std::string& text)
{
    logger.logInfo("Sending CallTalk to: ", to);
    common::OutgoingMessage msg{ common::MessageId::CallTalk, phoneNumber, to };
    msg.writeText(text);
    transport.sendMessage(msg.getMessage());
}

void BtsPort::sendCallAccepted(common::PhoneNumber to)
{
    logger.logInfo("Sending CallAccept to: ", to);
    common::OutgoingMessage msg{ common::MessageId::CallAccepted, phoneNumber, to };
    transport.sendMessage(msg.getMessage());
}

void BtsPort::sendUnknownRecipient(common::PhoneNumber to)
{
    logger.logInfo("Sending UnknownRecipient to: ", to);
    common::OutgoingMessage msg{ common::MessageId::UnknownRecipient, phoneNumber, to };
    transport.sendMessage(msg.getMessage());
}

}
