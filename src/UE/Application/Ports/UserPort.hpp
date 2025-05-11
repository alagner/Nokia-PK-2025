#pragma once

#include "IUserPort.hpp"
#include "Logger/PrefixedLogger.hpp"
#include "IUeGui.hpp"
#include "Messages/PhoneNumber.hpp"
#include "IEventsHandler.hpp"
#include "UeGui/ISmsComposeMode.hpp"
#include "SmsDb.hpp"
#include <optional>
#include <vector>

namespace ue
{

    class UserPort : public IUserPort
    {
    public:
        UserPort(common::ILogger &logger, IUeGui &gui, common::PhoneNumber phoneNumber);
        void start(IEventsHandler &handler);
        void stop();

        void showNotConnected() override;
        void showConnecting() override;
        void showConnected() override;
        void showNewSms() override;

        void showSmsList(const std::vector<SmsMessage> &messages);
        void showSmsView(const SmsMessage &message);
        void showAlert(const std::string &title, const std::string &message);
        void showSmsCompose() override;

        void showIncomingCall(common::PhoneNumber from) override;
        void showTalkingScreen(common::PhoneNumber peer) override;
        void showDialMode();

        common::PhoneNumber getSmsRecipient() const override;
        std::string getSmsText() const override;

        common::PhoneNumber getDialedPhoneNumber() const;

        void appendIncomingText(const std::string &text) override;
        void clearOutgoingText() override;
        std::string getCallText() const override;

    private:
        void acceptCallback();
        void rejectCallback();
        void mailCallback();

        common::PrefixedLogger logger;
        IUeGui &gui;
        common::PhoneNumber phoneNumber;
        IEventsHandler *handler = nullptr;
        details::GuiViewMode currentViewMode = details::VIEW_MODE_UNKNOWN;
        IUeGui::ISmsComposeMode *currentSmsMode = nullptr;
    };

}