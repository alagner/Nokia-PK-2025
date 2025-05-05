#pragma once

#include <gmock/gmock.h>
#include "Ports/ITimerPort.hpp"

namespace ue {

class ITimerEventsHandlerMock : public ITimerEventsHandler {
public:
    ITimerEventsHandlerMock();
    ~ITimerEventsHandlerMock() override;

    MOCK_METHOD(void, handleTimeout, (), (override));
};

class ITimerPortMock : public ITimerPort {
public:
    ITimerPortMock() = default;
    ~ITimerPortMock() override = default;

    MOCK_METHOD(void, startTimer, (Duration duration), (override));
    MOCK_METHOD(void, stopTimer, (), (override));
    MOCK_METHOD(void, setHandler, (ITimerEventsHandler* handler), (override));
};

} // namespace ue
