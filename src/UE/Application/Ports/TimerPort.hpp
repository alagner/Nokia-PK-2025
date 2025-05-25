#pragma once

#include "ITimerPort.hpp"
#include "Logger/PrefixedLogger.hpp"
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>

namespace ue
{

class TimerPort : public ITimerPort
{
public:
    TimerPort(common::ILogger& logger);
    ~TimerPort();

    void start(ITimerEventsHandler& handler);
    void stop();


    void startTimer(Duration duration) override;
    void stopTimer() override;

private:
    common::PrefixedLogger logger;
    ITimerEventsHandler* handler = nullptr;
    
    std::thread timerThread;
    std::mutex timerMutex;
    std::condition_variable timerCondition;
    std::atomic<bool> isTimerRunning{false};
    std::atomic<bool> shouldStop{false};
    
    void timerThreadFunction();
};

}
