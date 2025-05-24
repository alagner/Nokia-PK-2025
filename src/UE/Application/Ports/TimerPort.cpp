#include "TimerPort.hpp"

namespace ue
{

TimerPort::TimerPort(common::ILogger &logger)
    : logger(logger, "[TIMER PORT]")
{
    shouldStop = false;
    timerThread = std::thread(&TimerPort::timerThreadFunction, this);
}

TimerPort::~TimerPort()
{
    stopTimer();

    {
        std::lock_guard<std::mutex> lock(timerMutex);
        shouldStop = true;
    }
    timerCondition.notify_one();
    
    if (timerThread.joinable()) {
        timerThread.join();
    }
}

void TimerPort::start(ITimerEventsHandler &handler)
{
    logger.logDebug("Started");
    this->handler = &handler;
}

void TimerPort::stop()
{
    logger.logDebug("Stopped");
    stopTimer();
    handler = nullptr;
}

void TimerPort::startTimer(Duration duration)
{
    logger.logDebug("Start timer: ", duration.count(), "ms");
    
    std::lock_guard<std::mutex> lock(timerMutex);
    isTimerRunning = true;

    timerCondition.notify_one();
}

void TimerPort::stopTimer()
{
    logger.logDebug("Stop timer");
    
    std::lock_guard<std::mutex> lock(timerMutex);
    isTimerRunning = false;
}

void TimerPort::timerThreadFunction()
{
    logger.logDebug("Timer thread started");
    
    while (!shouldStop) {
        bool shouldNotify = false;
        
        {
            std::unique_lock<std::mutex> lock(timerMutex);
            timerCondition.wait(lock, [this] { return isTimerRunning || shouldStop; });
            
            if (shouldStop) {
                break;
            }
            
            if (isTimerRunning) {
                lock.unlock();
                std::this_thread::sleep_for(std::chrono::seconds(30));
                lock.lock();
                
                if (isTimerRunning) {  
                    shouldNotify = true;
                    isTimerRunning = false;  
                }
            }
        }
        
        if (shouldNotify && handler) {
            logger.logDebug("Timer expired, notifying handler");
            handler->handleTimeout();
        }
    }
    
    logger.logDebug("Timer thread stopped");
}

}
