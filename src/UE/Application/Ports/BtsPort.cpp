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
    transport.registerDisconnectedCallback([this]() {this->handler->handleDisconnect();}) ;
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
            auto text = reader.readRemainingText();
            handler->handleSms(from, text);
            break;
        }
        case common::MessageId::UnknownRecipient:
        {
            if (lastSentMessageType == common::MessageId::CallRequest)
            {
                handler->handleCallRecipientNotAvailable();
            }
            else if (lastSentMessageType == common::MessageId::CallTalk)
            {
                handler->handleCallRecipientNotAvailable();
            }
            else
            {
                handler->handleSmsDeliveryFailure(to);
            }
            break;
        }
        case common::MessageId::CallAccepted:
        {
            handler->handleCallAccepted(from);
            break;
        }
        case common::MessageId::CallDropped:
        {
            handler->handleCallDropped();
            break;
        }
        case common::MessageId::CallTalk:
        {
            auto text = reader.readRemainingText();
            handler->handleTalkMessage(from, text);
            break;
        }
        case common::MessageId::CallRequest:
        {
            handler->handleCallRequest(from);
            break;
        }
        default:
            logger.logError("unknow message: ", msgId, ", from: ", from);

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

void BtsPort::sendSms(const SmsEntity& sms)
{
    lastSentMessageType = common::MessageId::Sms;
    common::PhoneNumber phoneTo = static_cast<common::PhoneNumber>(sms.to);
    common::OutgoingMessage msg(common::MessageId::Sms, phoneNumber, phoneTo);
    msg.writeText(sms.text);
    transport.sendMessage(msg.getMessage());
}

void BtsPort::sendCallRequest(common::PhoneNumber from, common::PhoneNumber to)
{
    lastSentMessageType = common::MessageId::CallRequest;
    logger.logInfo("Sending CallRequest from ", from, " to ", to);
    common::OutgoingMessage msg(common::MessageId::CallRequest, from, to);
    transport.sendMessage(msg.getMessage());
}

void BtsPort::sendCallDropped(common::PhoneNumber from, common::PhoneNumber to)
{
    logger.logInfo("Sending CallDrop from ", from, " to ", to);
    common::OutgoingMessage msg(common::MessageId::CallDropped, from, to);
    transport.sendMessage(msg.getMessage());
}

void BtsPort::sendTalkMessage(common::PhoneNumber to, const std::string& text)
{
    lastSentMessageType = common::MessageId::CallTalk;
    common::OutgoingMessage msg(common::MessageId::CallTalk, phoneNumber, to);
    msg.writeText(text);
    transport.sendMessage(msg.getMessage());
}

void BtsPort::sendCallAccepted(common::PhoneNumber from)
{
    logger.logInfo("Sending CallAcceptted from ", phoneNumber, " to ", from);
    common::OutgoingMessage msg(common::MessageId::CallAccepted, phoneNumber, from);
    transport.sendMessage(msg.getMessage());
}

}