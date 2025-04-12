#pragma once

#include "IBtsPort.hpp"                     // Defines IBtsEventsHandler, IBtsPort
#include "Logger/PrefixedLogger.hpp"
#include "CommonEnvironment/ITransport.hpp" // Defines common::ITransport
#include "Messages/PhoneNumber.hpp"
#include "Messages/BinaryMessage.hpp"      
#include <string>

namespace ue
{


class BtsPort : public IBtsPort
{
public:
    BtsPort(common::ILogger& logger, common::ITransport& transport, common::PhoneNumber phoneNumber);
    void start(IBtsEventsHandler& handler);
    void stop();

    // IBtsPort interface overrides
    void sendAttachRequest(common::BtsId btsId) override;
    void sendSms(const common::PhoneNumber& recipient, const std::string& text) override;

    void sendCallRequest(const common::PhoneNumber& recipient);
    void sendCallAccepted(const common::PhoneNumber& recipient);
    void sendCallDropped(const common::PhoneNumber& recipient); 
    void sendCallTalk(const common::PhoneNumber& recipient, const std::string& text); 

private:
    void handleMessage(common::BinaryMessage msg);
    void handleDisconnect();

    common::PrefixedLogger logger;
    common::ITransport& transport;
    common::PhoneNumber phoneNumber; // Own number

    IBtsEventsHandler* handler = nullptr;
};

} // namespace ue