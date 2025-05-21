#pragma once
#include <gmock/gmock.h>
#include "Ports/IBtsPort.hpp"
#include "SmsRepository/SmsEntity.h"

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
    MOCK_METHOD(void, handleDisconnect, (), (final));
    MOCK_METHOD(void, handleSms, (common::PhoneNumber, std::string), (override));
    MOCK_METHOD(void, handleSmsDeliveryFailure, (common::PhoneNumber from), (override));
    MOCK_METHOD(void, handleCallAccepted, (), (override));
    MOCK_METHOD(void, handleCallDropped, (), (override));
    MOCK_METHOD(void, handleCallRecipientNotAvailable, (common::PhoneNumber from), (override));

};

class IBtsPortMock : public IBtsPort
{
public:
    IBtsPortMock();
    ~IBtsPortMock() override;

    MOCK_METHOD(void, sendAttachRequest, (common::BtsId), (final));
    MOCK_METHOD(void, sendSms, (const SmsEntity&), (final));
    MOCK_METHOD(void, sendCallRequest, (common::PhoneNumber, common::PhoneNumber), (override));
    MOCK_METHOD(void, sendCallDropped, (common::PhoneNumber from, common::PhoneNumber to), (override));
};

}
