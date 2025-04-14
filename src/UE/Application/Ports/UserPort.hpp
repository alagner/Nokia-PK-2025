#pragma once

#include "IUserPort.hpp"
#include "Logger/PrefixedLogger.hpp"
#include "IUeGui.hpp"
#include "Messages/PhoneNumber.hpp"
#include "IEventsHandler.hpp"
#include "UeGui/ISmsComposeMode.hpp"
#include <optional>
#include <vector>
#include "SmsStorage.hpp"

namespace ue{

class UserPort : public IUserPort{
public:
    UserPort(common::ILogger& logger, IUeGui& gui, common::PhoneNumber phoneNumber);
    void start(IEventsHandler& handler);
    void stop();

    void showNotConnected() override;
    void showConnecting() override;
    void showConnected() override;
    void showNewMessage() override;

    void showListMessage(const std::vector<SmsMessage> &messages);
    void showMessageView(const SmsMessage &message);
    void showNotify(const std::string &name, const std::string &message);
    void showMessageComp() override;

    common::PhoneNumber getMessageRecipient() const override;
    std::string getMessageText() const override;

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
