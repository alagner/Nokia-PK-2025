#pragma once

#include <gmock/gmock.h>
#include "Ports/IBtsPort.hpp"
#include "Messages/PhoneNumber.hpp"
#include <string>

namespace ue {

class IBtsPortMock : public IBtsPort
{
public:
    IBtsPortMock();
    ~IBtsPortMock() override;

    MOCK_METHOD(void, sendAttachRequest, (common::BtsId btsId), (override));
    MOCK_METHOD(void, sendSms, (const common::PhoneNumber& recipient, const std::string& text), (override));
    MOCK_METHOD(void, sendCallRequest, (const common::PhoneNumber&), (override));
    MOCK_METHOD(void, sendCallAccepted, (const common::PhoneNumber&), (override));
    MOCK_METHOD(void, sendCallDropped, (const common::PhoneNumber&), (override));
    MOCK_METHOD(void, sendCallTalk, (const common::PhoneNumber&, const std::string&), (override));
};

}