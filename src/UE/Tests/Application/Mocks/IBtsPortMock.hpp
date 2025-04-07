#pragma once

#include <gmock/gmock.h>
#include "Ports/IBtsPort.hpp"
#include "Messages/PhoneNumber.hpp" // Include for handleSms parameter
#include <string>              

namespace ue
{

class IBtsEventsHandlerMock : public IBtsEventsHandler
{
public:
    IBtsEventsHandlerMock();
    ~IBtsEventsHandlerMock() override;

    MOCK_METHOD(void, handleSib, (common::BtsId btsId), (override));
    MOCK_METHOD(void, handleAttachAccept, (), (override));
    MOCK_METHOD(void, handleAttachReject, (), (override));
    MOCK_METHOD(void, handleDisconnect, (), (override));
    MOCK_METHOD(void, handleSms, (const common::PhoneNumber& from, const std::string& text), (override));
};

class IBtsPortMock : public IBtsPort
{
public:
    IBtsPortMock();
    ~IBtsPortMock() override;

    MOCK_METHOD(void, sendAttachRequest, (common::BtsId btsId), (override));
    // Added MOCK_METHOD for sendSms
    MOCK_METHOD(void, sendSms, (const common::PhoneNumber& recipient, const std::string& text), (override));
};

}