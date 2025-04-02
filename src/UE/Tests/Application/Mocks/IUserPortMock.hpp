// Mocks/IUserPortMock.hpp
#pragma once

#include <gmock/gmock.h>
#include "Ports/IUserPort.hpp"
#include <vector>   // <-- Add include
#include <string>   // <-- Add include
// Need SmsMessage definition
#include "Application/SmsDb.hpp" // <-- Add include

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


    MOCK_METHOD(void, showSmsList, (const std::vector<SmsMessage>& messages), (override));
    MOCK_METHOD(void, showSmsView, (const SmsMessage& message), (override));
    MOCK_METHOD(void, showAlert, (const std::string& title, const std::string& message), (override));
    MOCK_METHOD(void, showSmsCompose, (), (override));
    MOCK_METHOD(common::PhoneNumber, getSmsRecipient, (), (const, override));
    MOCK_METHOD(std::string, getSmsText, (), (const, override));
};

} // namespace ue