#pragma once

#include "ITimerPort.hpp"
#include "Logger/PrefixedLogger.hpp"

namespace ue
{

class TimerPort : public ITimerPort
{
public:
    TimerPort(common::ILogger& logger);

    void start(ITimerEventsHandler& handler);
    void stop();

    // ITimerPort interface
    void startTimer(Duration duration) override;
    void stopTimer() override;
    void setHandler(ITimerEventsHandler* handler) override;
    void timeout();

private:
    common::PrefixedLogger logger;
    ITimerEventsHandler* handler = nullptr;
};

}
