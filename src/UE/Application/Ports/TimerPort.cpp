#include "TimerPort.hpp"
#include <thread>
#include <chrono>

namespace ue
{

    TimerPort::TimerPort(common::ILogger &logger)
        : logger(logger, "[TIMER PORT]"), isTimerActive(false), workerThread(nullptr)
    {
    }

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
        workerThread = std::make_unique<std::thread>(&TimerPort::runTimer, this, duration);
    }

    void TimerPort::stopTimer()
    {
        logger.logDebug("Stop timer");
        isTimerActive = false;
        joinThread();
    }

    void TimerPort::runTimer(Duration duration)
    {
        std::this_thread::sleep_for(duration);
        if (isTimerActive && handler != nullptr)
        {
            std::thread([handler = this->handler]()
                        { handler->handleTimeout(); })
                .detach();
        }
    }

    void TimerPort::joinThread()
    {
        if (workerThread && workerThread->joinable())
        {
            if (std::this_thread::get_id() != workerThread->get_id())
            {
                workerThread->join();
                workerThread.reset();
            }
        }
    }

    TimerPort::~TimerPort()
    {
        stopTimer();
    }
}
