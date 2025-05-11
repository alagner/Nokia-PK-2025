#pragma once

#include <gmock/gmock.h>
#include "Ports/IUserPort.hpp"
#include "Application/SmsDb.hpp"
#include <vector>
#include <string>

namespace ue
{

    class IUserEventsHandlerMock : public IUserEventsHandler
    {
    public:
        IUserEventsHandlerMock();
        ~IUserEventsHandlerMock() override;
    };

    class IUserPortMock : public IUserPort
    {
    public:
        IUserPortMock();
        ~IUserPortMock() override;

        MOCK_METHOD(void, showNotConnected, (), (override));
        MOCK_METHOD(void, showConnecting, (), (override));
        MOCK_METHOD(void, showConnected, (), (override));
        MOCK_METHOD(void, showNewSms, (), (override));

        MOCK_METHOD(void, showSmsList, (const std::vector<SmsMessage> &messages), (override));
        MOCK_METHOD(void, showSmsView, (const SmsMessage &message), (override));
        MOCK_METHOD(void, showAlert, (const std::string &title, const std::string &message), (override));
        MOCK_METHOD(void, showSmsCompose, (), (override));
        MOCK_METHOD(void, showIncomingCall, (common::PhoneNumber from), (override));
        MOCK_METHOD(void, showTalkingScreen, (common::PhoneNumber peer), (override));
        MOCK_METHOD(common::PhoneNumber, getSmsRecipient, (), (const, override));
        MOCK_METHOD(std::string, getSmsText, (), (const, override));
        MOCK_METHOD(void, showDialMode, (), (override));
        MOCK_METHOD(void, appendIncomingText, (const std::string &text), (override));
        MOCK_METHOD(void, clearOutgoingText, (), (override));
        MOCK_METHOD(std::string, getCallText, (), (const, override));
        MOCK_CONST_METHOD0(getDialedPhoneNumber, common::PhoneNumber());
    };

} // namespace ue