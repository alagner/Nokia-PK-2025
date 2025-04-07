#pragma once

#include "Logger/PrefixedLogger.hpp"
#include "Messages/PhoneNumber.hpp" // Include PhoneNumber
#include "IEventsHandler.hpp"
#include "Context.hpp"            // Context only needs forward declarations now
// REMOVED: #include "States/BaseState.hpp" // No longer needed here
#include "Data/SmsData.hpp"
#include <vector>
#include <string>
#include <memory> // For unique_ptr usage in Context (indirectly)


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
    ~Application(); // Declaration

    // --- Interface methods ---
    void handleTimeout() override;
    void handleSib(common::BtsId btsId) override;
    void handleAttachAccept() override;
    void handleAttachReject() override;
    void handleDisconnect() override;
    void handleSms(const common::PhoneNumber& from, const std::string& text) override;
    void handleUserAction(const std::string& id) override;

    // --- App specific methods ---
    void storeReceivedSms(const common::PhoneNumber& from, const std::string& text);
    void updateSmsIndicator();
    const std::vector<data::SmsData>& getSmsDb() const;
    void markSmsAsRead(std::size_t index);
    common::PhoneNumber getOwnPhoneNumber() const;


private:
    Context context; // Requires only forward declarations now via Context.hpp
    common::PrefixedLogger logger;
    std::vector<data::SmsData> smsDb;
    common::PhoneNumber phoneNumber;

};

inline common::PhoneNumber Application::getOwnPhoneNumber() const
{
    return phoneNumber;
}

} // namespace ue