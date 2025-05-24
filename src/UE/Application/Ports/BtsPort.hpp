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
    void sendSms(const SmsEntity& sms);
    void sendCallRequest(common::PhoneNumber from, common::PhoneNumber to) override;
    void sendCallDropped(common::PhoneNumber from, common::PhoneNumber to) override;
    void sendTalkMessage(common::PhoneNumber to, const std::string& text) override;
    void sendCallAccepted(common::PhoneNumber from) override;

private:
    void handleMessage(BinaryMessage msg);

    common::PrefixedLogger logger;
    common::ITransport& transport;
    common::PhoneNumber phoneNumber;
    common::MessageId lastSentMessageType = common::MessageId::Sib;

    IBtsEventsHandler* handler = nullptr;
};

}
