#include "TimerPort.hpp"

namespace ue
{

TimerPort::TimerPort(common::ILogger &logger)
    : logger(logger, "[TIMER PORT]")
{}

void TimerPort::start(ITimerEventsHandler &handler)
{
    logger.logDebug("Started");
    this->handler = &handler;
}

void TimerPort::stop()
{
    logger.logDebug("Stoped");
    handler = nullptr;
}

void TimerPort::startTimer(Duration duration)
{
    logger.logDebug("Start timer: ", duration.count(), "ms");

    timerActive = true;

    timerThread = std::thread([this, duration]() {
        std::this_thread::sleep_for(duration);
        if (timerActive && handler)
        {
            logger.logInfo("Timer expired – calling handleTimeout()");
            timerActive = false;
            handler->handleTimeout();
        }
    });

    timerThread.detach();
}

void TimerPort::startRedirectTimer(Duration duration)
{
    logger.logDebug("Start timer: ", duration.count(), "ms");

    timerActive = true;

    timerThread = std::thread([this, duration]() {
        std::this_thread::sleep_for(duration);
        if (timerActive && handler)
        {
            timerActive = false;
            handler->handleRedirect();
        }
    });

    timerThread.detach();
}

void TimerPort::stopTimer()
{
    logger.logDebug("Stop timer");
    timerActive = false;
}

}
