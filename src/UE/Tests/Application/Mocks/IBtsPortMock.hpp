#pragma once

#include <gmock/gmock.h>
#include "Ports/IBtsPort.hpp"
#include "Messages/PhoneNumber.hpp" // Include for handleSms parameter
#include <string>                   // Include for handleSms parameter

namespace ue
{

class IBtsEventsHandlerMock : public IBtsEventsHandler
{
public:
    // CORRECTED: Removed '= default' from declarations
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
    // CORRECTED: Removed '= default' from declarations
    IBtsPortMock();
    ~IBtsPortMock() override;


    MOCK_METHOD(void, sendAttachRequest, (common::BtsId btsId), (override));
};

} // namespace ue