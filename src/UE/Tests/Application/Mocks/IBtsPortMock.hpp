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
    MOCK_METHOD(void, handleSmsReceive, (common::PhoneNumber from, std::string messageText), (override));
    MOCK_METHOD(void, handleSmsSent, (common::PhoneNumber to, bool succeed), (override));
    MOCK_METHOD(void, handleCallRequest, (common::PhoneNumber), (override));

};

class IBtsPortMock : public IBtsPort
{
public:
    IBtsPortMock();
    ~IBtsPortMock() override;

    MOCK_METHOD(void, sendAttachRequest, (common::BtsId), (final));
    MOCK_METHOD(void, sendSms, (common::PhoneNumber to, const std::string& textMessage), (override));
    MOCK_METHOD(void, sendCallRequest, (common::PhoneNumber to), (override));

};

}
