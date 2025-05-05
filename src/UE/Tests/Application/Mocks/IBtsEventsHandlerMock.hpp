#pragma once

#include <gmock/gmock.h>
#include "Ports/IBtsPort.hpp"  // Ten plik powinien zawierać także definicję IBtsEventsHandler
#include "Messages/PhoneNumber.hpp"
#include "Messages/MessageId.hpp"
#include <string>

namespace ue {

class IBtsEventsHandlerMock : public IBtsEventsHandler {
public:
    IBtsEventsHandlerMock();
    ~IBtsEventsHandlerMock() override;

    MOCK_METHOD(void, handleSib, (common::BtsId btsId), (override));
    MOCK_METHOD(void, handleAttachAccept, (), (override));
    MOCK_METHOD(void, handleAttachReject, (), (override));
    MOCK_METHOD(void, handleDisconnect, (), (override));
    MOCK_METHOD(void, handleSms, (const common::PhoneNumber&, const std::string&), (override));
    MOCK_METHOD(void, handleCallRequest, (common::PhoneNumber), (override));
    MOCK_METHOD(void, handleCallAccepted, (common::PhoneNumber), (override));
    MOCK_METHOD(void, handleCallDropped, (common::PhoneNumber), (override));
    MOCK_METHOD(void, handleCallTalk, (common::PhoneNumber, const std::string&), (override));
    MOCK_METHOD(void, handleUnknownRecipient, (common::MessageId, common::PhoneNumber), (override));
};

} // namespace ue
