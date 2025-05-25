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
    MOCK_METHOD(void, startDial, (), (override));
    MOCK_METHOD(void, sendCallRequest, (common::PhoneNumber), (override));
    MOCK_METHOD(void, cancelCallRequest, (), (override));
    MOCK_METHOD(void, sendTalkMessage,(const std::string& text), (override));
    MOCK_METHOD(void, callAccept, (common::PhoneNumber), (override));
    MOCK_METHOD(void, callDrop, (), (override));
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
    MOCK_METHOD(void, startDial, (), (override));
    MOCK_METHOD(void, showDialing, (), (override));
    MOCK_METHOD(void, showTalking, (common:: PhoneNumber), (override));
    MOCK_METHOD(common::PhoneNumber, getPhoneNumber, (), (const));
    MOCK_METHOD(void, showPartnerNotAvailable,(common::PhoneNumber), (override));
    MOCK_METHOD(void, displayMessage, (common::PhoneNumber from, const std::string& text), (override));
    MOCK_METHOD(void, showCallRequest, (common::PhoneNumber), (override));
};

}
