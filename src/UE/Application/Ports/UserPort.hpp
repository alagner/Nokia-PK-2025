#pragma once

#include "IUserPort.hpp"
#include "Logger/PrefixedLogger.hpp"
#include "IUeGui.hpp"
#include "Messages/PhoneNumber.hpp"
#include "SmsRepository/SmsEntity.h"

namespace ue
{

class UserPort : public IUserPort
{
public:
    UserPort(common::ILogger& logger, IUeGui& gui, common::PhoneNumber phoneNumber);
    void start(IUserEventsHandler& handler);
    void stop();

    void showNotConnected() override;
    void showConnecting() override;
    void showConnected() override;
    void showNewSms() override;
    void showSms(const SmsEntity &) override;
    void showSmsList(const std::vector<SmsEntity> &) override;
    void composeSms() override;
    void startDial() override;
    void showDialing() override;
    void showTalking() override;
    common::PhoneNumber getPhoneNumber() const;
    void showPartnerNotAvailable() override;
    void displayMessage(common::PhoneNumber from, const std::string& text) override;
    void showCallRequest(common::PhoneNumber from) override;

private:
    common::PrefixedLogger logger;
    IUeGui& gui;
    common::PhoneNumber phoneNumber;
    IUserEventsHandler* handler = nullptr;
};

}
