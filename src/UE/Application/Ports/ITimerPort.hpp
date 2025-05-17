#pragma once

#include <chrono>
#include <thread>
#include <atomic>

namespace ue
{

class ITimerEventsHandler
{
public:
    virtual ~ITimerEventsHandler() = default;

    virtual void handleTimeout() = 0;
    virtual void handleRedirect() = 0;
};

class ITimerPort
{
public:
    using Duration = std::chrono::milliseconds;

    virtual ~ITimerPort() = default;

    virtual void startTimer(Duration) = 0;
    virtual void startRedirectTimer(Duration duration) = 0;
    virtual void stopTimer() = 0;

};

}
