#pragma once

#include "IUserPort.hpp"
#include "Logger/PrefixedLogger.hpp"
#include "IUeGui.hpp"
#include "UeGui/ISmsComposeMode.hpp"
#include "Messages/PhoneNumber.hpp"
#include "IEventsHandler.hpp"

namespace ue
{

class UserPort : public IUserPort
{
public:
    UserPort(common::ILogger& logger, IUeGui& gui, common::PhoneNumber phoneNumber);
    void start(IEventsHandler& handler);
    void stop();

    void showNotConnected() override;
    void showConnecting() override;
    void showConnected() override;
    void showSms() override;

    void showSmsList(const std::vector<SmsMessage>& textMessages) override;
    void showSmsView(const SmsMessage& textMessage) override;
    void showAlert(const std::string& title, const std::string& textMessage) override;
    void showSmsCompose() override;

    void showIncomingCall(common::PhoneNumber from) override;
    void showTalkingMobileScreen(common::PhoneNumber to) override;
    void showDialCompose() override;

    common::PhoneNumber getSmsRecipient() override;
    std::string getSmsTextMessage() override;

    common::PhoneNumber getDialedPhoneNumber() const override;

    void addIncomingText(const std::string& messageText) override;
    void deleteOutgoingText() override;
    std::string getTextFromCall() const override;


private:
    void onAccept(); // reff to callback
    void onReject();
    void onEnvelopeClicked();

    common::PrefixedLogger logger;
    IUeGui& gui;
    common::PhoneNumber phoneNumber;
    common::PhoneNumber dialedPhoneNumber;
    IEventsHandler* handler = nullptr;
    details::GuiViewMode currentViewMode = details::GuiViewMode::Unknown;
};

}
