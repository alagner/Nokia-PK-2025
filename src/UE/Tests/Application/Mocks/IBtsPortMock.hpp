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
        MOCK_METHOD(void, handleSmsReceived, (common::PhoneNumber from, std::string text), (override));
        MOCK_METHOD(void, handleSmsSentResult, (common::PhoneNumber to, bool success), (override));
    };

    class IBtsPortMock : public IBtsPort
    {
    public:
        IBtsPortMock();
        ~IBtsPortMock() override;

        MOCK_METHOD(void, sendAttachRequest, (common::BtsId), (final));
        MOCK_METHOD(void, sendSms, (common::PhoneNumber to, const std::string &text), (override));
        MOCK_METHOD(void, sendCallAccept, (common::PhoneNumber to), (override));
        MOCK_METHOD(void, sendCallReject, (common::PhoneNumber to), (override));
        MOCK_METHOD(void, sendCallDropped, (common::PhoneNumber to), (override));
        MOCK_METHOD(void, sendCallRequest, (common::PhoneNumber), (override));
        MOCK_METHOD(void, sendCallTalk, (common::PhoneNumber to, const std::string &text), (override));
    };

}
