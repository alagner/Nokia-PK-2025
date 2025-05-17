#pragma once

#include "IBtsPort.hpp"
#include "Logger/PrefixedLogger.hpp"
#include "ITransport.hpp"
#include "Messages/PhoneNumber.hpp"

namespace ue{

class BtsPort : public IBtsPort{
public:
    BtsPort(common::ILogger& logger, common::ITransport& transport, common::PhoneNumber phoneNumber);
    void start(IBtsEventsHandler& handler);
    void stop();

    void sendAttachRequest(common::BtsId) override;
    void sendMessage(common::PhoneNumber to, const std::string& text) override;
    void sendCallRequest(common::PhoneNumber to);
    void sendCallDropped(common::PhoneNumber to);
    void sendCallTalk(common::PhoneNumber to, const std::string& text);
    void sendCallAccepted(common::PhoneNumber to);
    void sendUnknownRecipient(common::PhoneNumber to);

private:
    void handleMessage(BinaryMessage msg);
    void handleDisconnected();

    common::PrefixedLogger logger;
    common::ITransport& transport;
    common::PhoneNumber phoneNumber;

    IBtsEventsHandler* handler = nullptr;
};

}
