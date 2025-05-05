#pragma once

#include "IEventsHandler.hpp"
#include "IUeGui.hpp"
#include "IUserPort.hpp"
#include "Logger/PrefixedLogger.hpp"
#include "Messages/PhoneNumber.hpp"
#include "SmsStorage.hpp"
#include "UeGui/ISmsComposeMode.hpp"
#include <optional>
#include <vector>

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
    void showNewMessage() override;

    void showListMessage(const std::vector<SmsMessage>& messages);
    void showMessageView(const SmsMessage& message);
    void showNotify(const std::string& name, const std::string& message);
    void showMessageComp() override;

    void showIncomingCall(const common::PhoneNumber& caller) override;
    void showCallInProgress(const common::PhoneNumber& otherPhoneNumber) override;
    void showEndedCall(const common::PhoneNumber& otherPhoneNumber, const std::string& reason) override;
    void showCallFailed(const common::PhoneNumber& otherPhoneNumber, const std::string& errorMessage) override;

    void showCallMenu() override;

    common::PhoneNumber getMessageRecipient() const override;
    std::string getMessageText() const override;
    common::PhoneNumber getCallRecipient() const override;

private:
    void acceptCallback();
    void rejectCallback();
    void messageCallback();

    common::PrefixedLogger logger;
    IUeGui& gui;
    common::PhoneNumber phoneNumber;
    IEventsHandler* handler = nullptr;
    view_details::GuiViewMode currentViewMode = view_details::VM_DEFAULT;
};

}
