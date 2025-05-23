#pragma once

#include "IBtsPort.hpp"
#include "Logger/PrefixedLogger.hpp"
#include "ITransport.hpp"
#include "Messages/PhoneNumber.hpp"

namespace ue
{

class BtsPort : public IBtsPort
{
public:
    BtsPort(common::ILogger& logger, common::ITransport& transport, common::PhoneNumber phoneNumber);
    void start(IBtsEventsHandler& handler);
    void stop();

    void sendAttachRequest(common::BtsId) override;
    void sendSms(common::PhoneNumber to, const std::string& textMessage) override;
    void sendCallRequest(common::PhoneNumber to) override;
    void sendCallDropped(common::PhoneNumber to) override;
    void sendAcceptCall(common::PhoneNumber to) override;
    void sendRejectCall(common::PhoneNumber to) override;
    void sendTalkCall(common::PhoneNumber to, const std::string& message) override;

private:
    void handleMessage(BinaryMessage msg);
    void handleDisconnected();

    common::PrefixedLogger logger;
    common::ITransport& transport;
    common::PhoneNumber phoneNumber;

    IBtsEventsHandler* handler = nullptr;

};

}
