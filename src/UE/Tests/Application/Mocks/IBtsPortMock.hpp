#pragma once

#include <gmock/gmock.h>
#include "Ports/IBtsPort.hpp"

namespace ue
{

class IBtsEventsHandlerMock : public IBtsEventsHandler
{
public:
    IBtsEventsHandlerMock();
    ~IBtsEventsHandlerMock() override;

    MOCK_METHOD(void, handleSib, (common::BtsId), (final));
    MOCK_METHOD(void, handleAttachAccept, (), (final));
    MOCK_METHOD(void, handleAttachReject, (), (final));
    MOCK_METHOD(void, handleDisconnected, (), (final));
    MOCK_METHOD(void, handleMessageReceive, (common::PhoneNumber sender, std::string text), (override));
    MOCK_METHOD(void, handleMessageSentResult, (common::PhoneNumber to, bool success), (final));
    MOCK_METHOD(void, handleCallRequest, (common::PhoneNumber from), (override));
};

class IBtsPortMock : public IBtsPort
{
public:
    IBtsPortMock();
    ~IBtsPortMock() override;

    MOCK_METHOD(void, sendAttachRequest, (common::BtsId), (final));
    MOCK_METHOD(void, sendMessage, (common::PhoneNumber to, const std::string& text), (final));
    MOCK_METHOD(void, sendCallRequest, (common::PhoneNumber to));
    MOCK_METHOD(void, sendCallDropped, (common::PhoneNumber to));
    MOCK_METHOD(void, sendCallTalk, (common::PhoneNumber to, const std::string& text));
    MOCK_METHOD(void, sendCallAccepted, (common::PhoneNumber to));
    MOCK_METHOD(void, sendUnknownRecipient, (common::PhoneNumber to));

};

}
