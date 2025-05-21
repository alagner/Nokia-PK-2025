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
    
    MOCK_METHOD(void, acceptCallRequest, (), (override));
    MOCK_METHOD(void, rejectCallRequest, (), (override));
    MOCK_METHOD(void, dial, (), (override));
    MOCK_METHOD(void, acceptDialing, (common::PhoneNumber), (override));
    MOCK_METHOD(void, rejectDialing, (), (override));
    MOCK_METHOD(void, viewSms, (), (override));
    MOCK_METHOD(void, selectSms, (size_t), (override));
    MOCK_METHOD(void, closeSmsView, (), (override));
    MOCK_METHOD(void, composeSms, (), (override));
    MOCK_METHOD(void, acceptSmsCompose, (common::PhoneNumber, const std::string&), (override));
    MOCK_METHOD(void, rejectSmsCompose, (), (override));
};

class IUserPortMock : public IUserPort
{
public:
    IUserPortMock();
    ~IUserPortMock() override;

    MOCK_METHOD(void, showNotConnected, (), (final));
    MOCK_METHOD(void, showConnecting, (), (final));
    MOCK_METHOD(void, showConnected, (), (final));
    MOCK_METHOD(void, showNewSms, (bool), (override));
    MOCK_METHOD(void, showSmsList, (), (override));
    MOCK_METHOD(void, showSmsContent, (const std::string&, const std::string&), (override));
    MOCK_METHOD(void, showSentSmsContent, (const std::string&, const std::string&), (override));
    MOCK_METHOD(void, showCallRequest, (common::PhoneNumber), (override));
    MOCK_METHOD(void, showCallView, (common::PhoneNumber), (override));
    MOCK_METHOD(void, showCallMessage, (std::string), (override));
    MOCK_METHOD(void, showPeerUserNotAvailable, (common::PhoneNumber), (override));
    MOCK_METHOD(void, showDialView, (), (override));
    MOCK_METHOD(void, showSmsComposeView, (), (override));

};
}
