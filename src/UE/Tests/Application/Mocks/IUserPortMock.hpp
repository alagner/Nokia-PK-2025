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

    MOCK_METHOD(void, viewSmsList, (), (final));
    MOCK_METHOD(void, viewSms, (unsigned), (final));
    MOCK_METHOD(void, sendSms, (const SmsEntity&),(final));
    MOCK_METHOD(void, composeSms,(),(final));
};

class IUserPortMock : public IUserPort
{
public:
    IUserPortMock();
    ~IUserPortMock() override;

    MOCK_METHOD(void, showNotConnected, (), (final));
    MOCK_METHOD(void, showConnecting, (), (final));
    MOCK_METHOD(void, showConnected, (), (final));
    MOCK_METHOD(void, showNewSms, (), (override));
    MOCK_METHOD(void, showSmsList, (const std::vector<SmsEntity> &), (override));
    MOCK_METHOD(void, showSms, (const SmsEntity &), (override));
    MOCK_METHOD(void, composeSms, (), (final));
    MOCK_METHOD(common::PhoneNumber, getPhoneNumber, (), (const));
};

}
