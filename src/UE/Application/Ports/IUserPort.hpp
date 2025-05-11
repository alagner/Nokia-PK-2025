#pragma once

#include <vector>
#include <string>
#include "SmsDb.hpp"

namespace ue
{

    class IUserEventsHandler
    {
    public:
        virtual ~IUserEventsHandler() = default;
    };

    class IUserPort
    {
    public:
        virtual ~IUserPort() = default;

        virtual void showNotConnected() = 0;
        virtual void showConnecting() = 0;
        virtual void showConnected() = 0;
        virtual void showNewSms() = 0;

        virtual void showSmsList(const std::vector<SmsMessage> &messages) = 0;
        virtual void showSmsView(const SmsMessage &message) = 0;
        virtual void showAlert(const std::string &title, const std::string &message) = 0;
        virtual void showSmsCompose() = 0;
        virtual void showDialMode() = 0;

        virtual void showIncomingCall(common::PhoneNumber from) = 0;
        virtual void showTalkingScreen(common::PhoneNumber peer) = 0;

        virtual common::PhoneNumber getSmsRecipient() const = 0;
        virtual std::string getSmsText() const = 0;

        virtual common::PhoneNumber getDialedPhoneNumber() const = 0;

        virtual void appendIncomingText(const std::string &text) = 0;
        virtual void clearOutgoingText() = 0;
        virtual std::string getCallText() const = 0;
    };

}
