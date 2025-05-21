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
};

class IUserPortMock : public IUserPort
{
public:
    IUserPortMock();
    ~IUserPortMock() override;

    MOCK_METHOD(void, showNotConnected, (), (final));
    MOCK_METHOD(void, showConnecting, (), (final));
    MOCK_METHOD(void, showConnected, (), (final));
    MOCK_METHOD(void, showNewSms, (bool), (override));  // Fixed to include boolean parameter
    MOCK_METHOD(void, showCallRequest, (common::PhoneNumber), (override));
    MOCK_METHOD(void, showCallView, (common::PhoneNumber), (override));
    MOCK_METHOD(void, showCallMessage, (std::string), (override));
    MOCK_METHOD(void, showPeerUserNotAvailable, (common::PhoneNumber), (override));
    MOCK_METHOD(void, showDialView, (), (override));
<<<<<<< Updated upstream
};
=======
    MOCK_METHOD(void, showSmsComposeView, (), (override));
    MOCK_METHOD(std::string, getOutgoingCallText, (), (override));
    MOCK_METHOD(void, clearOutgoingCallText, (), (override));
    MOCK_METHOD(void, clearIncomingCallText, (), (override));
>>>>>>> Stashed changes

};
}
