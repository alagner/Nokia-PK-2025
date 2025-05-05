#pragma once

#include "Logger/PrefixedLogger.hpp"
#include "Messages/PhoneNumber.hpp"
#include "IEventsHandler.hpp"
#include "Context.hpp"
#include "States/BaseState.hpp"
#include "Messages/MessageId.hpp"
#include "Data/SmsData.hpp"
#include <vector>
#include <string>
#include <memory>


namespace ue
{

using common::PhoneNumber;
using common::ILogger;

class Application : public IEventsHandler
{
public:
    Application(PhoneNumber phoneNumber,
                ILogger& iLogger,
                IBtsPort& bts,
                IUserPort& user,
                ITimerPort& timer);
    ~Application();

    // --- Interface methods ---
    void handleTimeout() override;
    void handleSib(common::BtsId btsId) override;
    void handleAttachAccept() override;
    void handleAttachReject() override;
    void handleDisconnect() override;
    void handleSms(const common::PhoneNumber& from, const std::string& text) override;
    void handleUserAction(const std::string& id) override;

    void handleCallRequest(common::PhoneNumber from) override;
    void handleCallAccepted(common::PhoneNumber from) override;
    void handleCallDropped(common::PhoneNumber from) override;
    void handleCallTalk(common::PhoneNumber from, const std::string& text) override;
    void handleUnknownRecipient(common::MessageId msgId, common::PhoneNumber from) override;

    // --- App specific methods ---
    void storeReceivedSms(const common::PhoneNumber& from, const std::string& text);
    // Added for Sending SMS
    void storeSentSms(const common::PhoneNumber& to, const std::string& text);
    void updateSmsIndicator();
    const std::vector<data::SmsData>& getSmsDb() const;
    void markSmsAsRead(std::size_t index);
    common::PhoneNumber getOwnPhoneNumber() const;

    Context& getContext();

private:
    Context context;
    common::PrefixedLogger logger;
    std::vector<data::SmsData> smsDb;
    common::PhoneNumber phoneNumber;

};

inline common::PhoneNumber Application::getOwnPhoneNumber() const
{
    return phoneNumber;
}

}
