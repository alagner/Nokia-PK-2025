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

class IUserPortMock : public IUserPort
{
public:
    IUserPortMock();
    ~IUserPortMock() override;

    MOCK_METHOD(void, showNotConnected, (), (final));
    MOCK_METHOD(void, showConnecting, (), (final));
    MOCK_METHOD(void, showConnected, (), (final));
    MOCK_METHOD(void, showNewMessage, (), (override));

    MOCK_METHOD(void, showListMessage, (const std::vector<SmsMessage>& messages), (override));
    MOCK_METHOD(void, showMessageView, (const SmsMessage& message), (override));
    MOCK_METHOD(void, showNotify, (const std::string& name, const std::string& message), (final));
    MOCK_METHOD(void, showMessageComp, (), (final));

    MOCK_METHOD(common::PhoneNumber, getMessageRecipient, (), (const, final));
    MOCK_METHOD(std::string, getMessageText, (), (const, final));
};

}
