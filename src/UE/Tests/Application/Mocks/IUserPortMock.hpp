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

    MOCK_METHOD(void, showNotConnected, (), (override)); // Use override
    MOCK_METHOD(void, showConnecting, (), (override));   // Use override
    MOCK_METHOD(void, showConnected, (), (override));    // Use override
    MOCK_METHOD(void, showNewSms, (), (override));       // Use override

    // --- Add Mocks for the new methods ---
    MOCK_METHOD(void, showSmsList, (const std::vector<SmsMessage>& messages), (override));
    MOCK_METHOD(void, showSmsView, (const SmsMessage& message), (override));
    MOCK_METHOD(void, showAlert, (const std::string& title, const std::string& message), (override));
    // --- End of added mocks ---
};

} // namespace ue