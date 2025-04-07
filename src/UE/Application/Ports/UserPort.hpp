#pragma once

#include "IUserPort.hpp"
#include "Logger/PrefixedLogger.hpp"
#include "IUeGui.hpp"
#include "Messages/PhoneNumber.hpp"
#include "Data/SmsData.hpp" // Include SmsData for displaySmsList/viewSms
#include <vector>          // Include vector

namespace ue
{

class UserPort : public IUserPort
{
public:
    UserPort(common::ILogger& logger, IUeGui& gui, common::PhoneNumber phoneNumber);
    void start(IUserEventsHandler& handler);
    void stop();

    // IUserPort interface
    void showNotConnected() override;
    void showConnecting() override;
    void showConnected() override;
    // CORRECTED: Override matches IUeGui::showNewSms
    void showNewSms(bool present) override;
    void displaySmsList(const std::vector<data::SmsData>& smsList) override; // Added for View SMS
    void viewSms(const data::SmsData& sms) override; // Added for View SMS

private:
    void handleMenuSelection(const std::string& id); // Added for View SMS

    common::PrefixedLogger logger;
    IUeGui& gui;
    common::PhoneNumber phoneNumber;
    IUserEventsHandler* handler = nullptr;
};

}