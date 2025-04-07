#include "BtsPort.hpp"
#include "Messages/IncomingMessage.hpp"
#include "Messages/OutgoingMessage.hpp"
#include "Messages/MessageId.hpp" // Ensure MessageId enum is included

#include <vector>  // Include for std::vector
#include <string>  // Include for std::string

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
        auto to = reader.readPhoneNumber(); // Read 'to' even if not always used by handler

        // Basic check if the message is for this UE (can be refined)
        if (to != phoneNumber and
            msgId != common::MessageId::Sib and // SIB is broadcast
            msgId != common::MessageId::AttachResponse // AttachResponse is for us
           )
        {
             logger.logInfo("Received message addressed to different UE (", to, "), ignoring. MsgId: ", msgId);
             return;
        }


        switch (msgId)
        {
        case common::MessageId::Sib:
        {
            auto btsId = reader.readBtsId();
            if (handler) handler->handleSib(btsId);
            break;
        }
        case common::MessageId::AttachResponse:
        {
            bool accept = reader.readNumber<std::uint8_t>() != 0u;
            if (handler)
            {
                if (accept)
                    handler->handleAttachAccept();
                else
                    handler->handleAttachReject();
            }
            break;
        }
        case common::MessageId::Sms: // Handle incoming SMS
        {
            // SMS format (Table 8): Header, encryption (optional, skip for now), text
            // Assuming no encryption for now as per basic feature requirements.
            // CORRECTED: Use readRemainingText()
            std::string text = reader.readRemainingText();
            logger.logDebug("Received SMS from ", from, " with text: ", text);
            if (handler) handler->handleSms(from, text);
            break;
        }
        // Add cases for other message types later (CallRequest, CallAccept, etc.)
        case common::MessageId::UnknownRecipient: // Log errors as per spec 3.4
        case common::MessageId::UnknownSender:
            logger.logError("Received error message from BTS: ", msgId, ", original sender: ", from);
            // Optionally parse the failed message header if needed for context
            break;
        default:
            logger.logError("Unknown message received: ", msgId, ", from: ", from, ", to: ", to);
            break; // Use break instead of logError return

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
                                common::PhoneNumber{}}; // 'to' field not used for AttachRequest
    msg.writeBtsId(btsId);
    transport.sendMessage(msg.getMessage());
}

void BtsPort::handleDisconnect()
{
        logger.logInfo("Disconnected from BTS");
        if (handler) handler->handleDisconnect();
}

}