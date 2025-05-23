#include "TimerPort.hpp"

namespace ue {
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
        stopTimer();
        handler = nullptr;
    }

    void TimerPort::startTimer(Duration duration)
    {
        logger.logDebug("Start timer: ", duration.count(), "ms");
        stopTimer();

        isTimerActive = true;
        std::thread(&TimerPort::runTimer, this, duration).detach();
    }

    void TimerPort::stopTimer()
    {
        logger.logDebug("Stop timer");
        isTimerActive = false;
    }

    void TimerPort::runTimer(Duration duration)
    {
        std::this_thread::sleep_for(duration);
        if (isTimerActive)
        {
            logger.logDebug("Timer expired");
            handler->handleTimeout();
        }
        else
        {
            logger.logDebug("Timer stopped before expiration");
        }

    }

    TimerPort::~TimerPort() {
        stopTimer();
    }

}
