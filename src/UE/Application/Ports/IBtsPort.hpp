#pragma once

#include "Messages/BtsId.hpp"
#include "Messages/PhoneNumber.hpp"
#include <string>

namespace ue
{

    class IBtsEventsHandler
    {
    public:
        virtual ~IBtsEventsHandler() = default;

        virtual void handleSib(common::BtsId) = 0;
        virtual void handleAttachAccept() = 0;
        virtual void handleAttachReject() = 0;
        virtual void handleDisconnected() = 0;
        virtual void handleSmsReceived(common::PhoneNumber from, std::string text) = 0;
        virtual void handleSmsSentResult(common::PhoneNumber to, bool success) = 0;
        virtual void handleCallRequest(common::PhoneNumber from) = 0;
        virtual void handleCallDropped(common::PhoneNumber peer) = 0;
        virtual void handleCallAccept(common::PhoneNumber peer) = 0;
        virtual void handleUnknownRecipient(common::PhoneNumber peer) = 0;
        virtual void handleCallTalk(common::PhoneNumber from, const std::string &text) = 0;
        virtual void handleCallReject(common::PhoneNumber from) = 0;
    };

    class IBtsPort
    {
    public:
        virtual ~IBtsPort() = default;

        virtual void sendAttachRequest(common::BtsId) = 0;
        virtual void sendSms(common::PhoneNumber to, const std::string &text) = 0;
        virtual void sendCallAccept(common::PhoneNumber to) = 0;
        virtual void sendCallReject(common::PhoneNumber to) = 0;
        virtual void sendCallDropped(common::PhoneNumber to) = 0;
        virtual void sendCallRequest(common::PhoneNumber to) = 0;
        virtual void sendCallTalk(common::PhoneNumber to, const std::string &text) = 0;
    };

}
