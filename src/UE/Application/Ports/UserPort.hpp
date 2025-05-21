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
<<<<<<< Updated upstream
=======
    IUeGui::ICallMode& getCallMode();
    

    void setSmsList(const std::vector<Sms>& smsList) override;
    void setSelectSmsCallback(std::function<void(size_t)> callback) override;
>>>>>>> Stashed changes
    
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
<<<<<<< Updated upstream
=======
    void handleViewSmsClicked();
    void handleComposeSmsClicked();
    void handleAcceptSmsComposeClicked();
    void handleRejectSmsComposeClicked();
    void handleSelectSmsClicked(size_t index);
    void handleCloseSmsViewClicked();
    std::string getOutgoingCallText() override;
    void clearOutgoingCallText() override;
    void clearIncomingCallText() override;
>>>>>>> Stashed changes
};

}
