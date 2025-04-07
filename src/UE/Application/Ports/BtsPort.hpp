#pragma once

#include "IBtsPort.hpp"                     // Defines IBtsEventsHandler, IBtsPort
#include "Logger/PrefixedLogger.hpp"
#include "CommonEnvironment/ITransport.hpp" // Defines common::ITransport
#include "Messages/PhoneNumber.hpp"
#include "Messages/BinaryMessage.hpp"       // CORRECTED: Include definition of common::BinaryMessage
#include <string>

namespace ue
{

// REMOVED Incorrect using alias:
// using BinaryMessage = common::ITransport::BinaryMessage;

class BtsPort : public IBtsPort
{
public:
    BtsPort(common::ILogger& logger, common::ITransport& transport, common::PhoneNumber phoneNumber);
    void start(IBtsEventsHandler& handler);
    void stop();

    // IBtsPort interface overrides
    void sendAttachRequest(common::BtsId btsId) override;
    void sendSms(const common::PhoneNumber& recipient, const std::string& text) override;

private:
    // CORRECTED: Use correct type common::BinaryMessage
    void handleMessage(common::BinaryMessage msg);
    void handleDisconnect();

    common::PrefixedLogger logger;
    common::ITransport& transport;
    common::PhoneNumber phoneNumber; // Own number

    IBtsEventsHandler* handler = nullptr;
};

} // namespace ue