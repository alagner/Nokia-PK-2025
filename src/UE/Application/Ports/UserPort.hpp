#pragma once

#include "IUserPort.hpp"
#include "ISmsListViewPort.hpp"
#include "Logger/PrefixedLogger.hpp"
#include "IUeGui.hpp"
#include "Messages/PhoneNumber.hpp"

namespace ue
{

class UserPort : public IUserPort, public ISmsListViewPort
{
public:
    UserPort(common::ILogger& logger, IUeGui& gui, common::PhoneNumber phoneNumber);
    void start(IUserEventsHandler& handler);
    void stop();

    void showNotConnected() override;
    void showConnecting() override;
    void showConnected() override;
    void showNewSms(bool present) override;
    void showSmsList() override;
    void showSmsContent(const std::string& from, const std::string& text) override;
    void showSentSmsContent(const std::string& to, const std::string& text) override;
    void showSmsComposeView() override;
    

    void showCallRequest(common::PhoneNumber phoneNumber) override;
    void showCallView(common::PhoneNumber phoneNumber) override;
    void showCallMessage(std::string message) override;
    void showPeerUserNotAvailable(common::PhoneNumber phoneNumber) override;
    

    void showDialView() override;

    IUeGui::ICallMode& getCallMode();
    

    void setSmsList(const std::vector<Sms>& smsList) override;
    void setSelectSmsCallback(std::function<void(size_t)> callback) override;

    
private:
    common::PrefixedLogger logger;
    IUeGui& gui;
    common::PhoneNumber phoneNumber;
    IUserEventsHandler* handler = nullptr;
    

    std::vector<Sms> currentSmsList;
    std::function<void(size_t)> selectSmsCallback;
    

    void handleAcceptCallClicked();
    void handleRejectCallClicked();
    void handleDialClicked();
    void handleAcceptDialingClicked();
    void handleRejectDialingClicked();

    void handleViewSmsClicked();
    void handleComposeSmsClicked();
    void handleAcceptSmsComposeClicked();
    void handleRejectSmsComposeClicked();
    void handleSelectSmsClicked(size_t index);
    void handleCloseSmsViewClicked();

    std::string getOutgoingCallText() override;
    void clearOutgoingCallText() override;
    void clearIncomingCallText() override;

};

}
