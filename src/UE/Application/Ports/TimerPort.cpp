#include "TimerPort.hpp"
#include <thread>
#include <chrono>  

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
    std::thread([this, duration]() {
        std::this_thread::sleep_for(duration);
        if (handler) {
            logger.logDebug("Timer expired, calling handleTimeout()");
            handler->handleTimeout();
        }
    }).detach();
}

void TimerPort::stopTimer()
{
    logger.logDebug("Stop timer");
}

void TimerPort::timeout()
{
    if (handler)
        handler->handleTimeout(); 
}

void TimerPort::setHandler(ITimerEventsHandler* h)
{
    this->handler = h;
}

}
