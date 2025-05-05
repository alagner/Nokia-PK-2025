#pragma once

#include "IUserPort.hpp"
#include "Logger/PrefixedLogger.hpp"
#include "IUeGui.hpp"
#include "Messages/PhoneNumber.hpp"

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
    void showNewSms(bool present) override;  // Updated to match interface
    
    // Call-related UI methods
    void showCallRequest(common::PhoneNumber phoneNumber) override;
    void showCallView(common::PhoneNumber phoneNumber) override;
    void showCallMessage(std::string message) override;
    void showPeerUserNotAvailable(common::PhoneNumber phoneNumber) override;
    
    // Dial-related UI methods
    void showDialView() override;
    
private:
    common::PrefixedLogger logger;
    IUeGui& gui;
    common::PhoneNumber phoneNumber;
    IUserEventsHandler* handler = nullptr;
    
    // Callbacks for UI events
    void handleAcceptCallClicked();
    void handleRejectCallClicked();
    void handleDialClicked();
    void handleAcceptDialingClicked();
    void handleRejectDialingClicked();
};

}
