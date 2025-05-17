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
    void startRedirectTimer(Duration duration) override;

private:
    common::PrefixedLogger logger;
    ITimerEventsHandler* handler = nullptr;
    std::atomic<bool> timerActive{false};
    std::thread timerThread;
};

}
