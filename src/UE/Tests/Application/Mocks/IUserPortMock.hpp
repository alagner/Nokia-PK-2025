#pragma once

#include <gmock/gmock.h>
#include "Ports/IUserPort.hpp"

namespace ue
{

    class IUserEventsHandlerMock : public IUserEventsHandler
    {
    public:
        IUserEventsHandlerMock();
        ~IUserEventsHandlerMock() override;
    };

    // Main mock for user interface port
    class IUserPortMock : public IUserPort
    {
    public:
        IUserPortMock();
        ~IUserPortMock() override;

        // Connection status
        MOCK_METHOD(void, showNotConnected, (), (override));
        MOCK_METHOD(void, showConnecting, (), (override));
        MOCK_METHOD(void, showConnected, (), (override));

        // SMS-related views
        MOCK_METHOD(void, showSms, (), (override)); // <--- brakowało!
        MOCK_METHOD(void, showSmsList, (const std::vector<SmsMessage>& textMessages), (override)); // <--- brakowało!
        MOCK_METHOD(void, showSmsView, (const SmsMessage& textMessage), (override)); // <--- brakowało!
        MOCK_METHOD(void, showSmsCompose, (), (override));
        MOCK_METHOD(common::PhoneNumber, getSmsRecipient, (), (override));
        MOCK_METHOD(std::string, getSmsTextMessage, (), (override));

        // Generic alert view
        MOCK_METHOD(void, showAlert, (const std::string& title, const std::string& textMessage), (override));
    };

} // namespace ue
