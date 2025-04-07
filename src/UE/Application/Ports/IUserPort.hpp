#pragma once

#include "Messages/PhoneNumber.hpp" // Include for PhoneNumber
#include "Data/SmsData.hpp"       // CORRECTED: Include SmsData definition
#include <vector>                 // CORRECTED: Include vector for std::vector
#include <string>                 // Include string for handleUserAction

namespace ue
{

// Forward declaration (if needed, though SmsData.hpp is included)
// namespace data { struct SmsData; }

class IUserEventsHandler
{
public:
    virtual ~IUserEventsHandler() = default;
    // Added handleUserAction for menu selections etc.
    virtual void handleUserAction(const std::string& id) = 0;
};

class IUserPort
{
public:
    virtual ~IUserPort() = default;

    virtual void showNotConnected() = 0;
    virtual void showConnecting() = 0;
    virtual void showConnected() = 0;
    // Method signature matches IUeGui::showNewSms
    virtual void showNewSms(bool present) = 0;
    // Methods added for View SMS feature
    virtual void displaySmsList(const std::vector<data::SmsData>& smsList) = 0;
    virtual void viewSms(const data::SmsData& sms) = 0;
};

} // namespace ue