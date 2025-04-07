#pragma once

#include <gmock/gmock.h>
#include "Ports/IUserPort.hpp"
#include <string> // Include for handleUserAction parameter

namespace ue
{

class IUserEventsHandlerMock : public IUserEventsHandler
{
public:
    // Keep existing constructor/destructor declarations if defined in .cpp
    IUserEventsHandlerMock();
    ~IUserEventsHandlerMock() override;

    // CORRECTED: Added MOCK_METHOD for handleUserAction
    MOCK_METHOD(void, handleUserAction, (const std::string& id), (override));
};

class IUserPortMock : public IUserPort
{
public:
    // Keep existing constructor/destructor declarations if defined in .cpp
    IUserPortMock();
    ~IUserPortMock() override;

    MOCK_METHOD(void, showNotConnected, (), (override));
    MOCK_METHOD(void, showConnecting, (), (override));
    MOCK_METHOD(void, showConnected, (), (override));
    // Need mocks for the other IUserPort methods added previously
    MOCK_METHOD(void, showNewSms, (bool present), (override));
    MOCK_METHOD(void, displaySmsList, (const std::vector<data::SmsData>& smsList), (override));
    MOCK_METHOD(void, viewSms, (const data::SmsData& sms), (override));
};

} // namespace ue