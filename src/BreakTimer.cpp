#include "BreakTimer.h"

const auto kIdleDurationToPause = std::chrono::seconds(60);

BreakTimer::BreakTimer(Type type, int hour, int min, int sec) 
    : QObject(nullptr), type_(type)
{
    // Starting timer
    setupDuration_ = std::chrono::hours(hour) 
        + std::chrono::minutes(min) 
        + std::chrono::seconds(sec);
    timeLeft_ = setupDuration_;

    state_ = State::Running;
    startProperWorker();
    logInfo("Timer started");
}

BreakTimer::~BreakTimer() 
{
    // Stopping timer
    logInfo("Stopped");
    state_ = State::Stopped;
    condVar_.notify_one();
    if (backgroundThread_.joinable())
        backgroundThread_.join();
}

void BreakTimer::restart(const bool noLog) 
{
    if (!noLog)
        logInfo("Restarted");    
    state_ = State::Running;
    timeLeft_ = setupDuration_;
    condVar_.notify_one();
}

void BreakTimer::pause(const bool noLog) 
{
    if (!noLog)
        logInfo("Paused");
    state_ = State::Paused;
    condVar_.notify_one();
}

void BreakTimer::resume(const bool noLog) 
{
    if (!noLog)
        logInfo("Resumed");
    state_ = State::Running;
    condVar_.notify_one();
}

std::chrono::system_clock::duration BreakTimer::getTimeLeft() 
{
    return timeLeft_.load();
}

void BreakTimer::interactiveBackgroundWorker() 
{
    auto prevCheck = std::chrono::system_clock::now();
    bool sleepOnly = false;
    while (state_ != State::Stopped) {
        try {
            const auto& pe = PlatformEvents::instance();
            const auto idleTime = pe.getIdleTime();
            switch (state_) {
            case State::Running:
                if (idleTime > kIdleDurationToPause)
                    sleepOnly = true;

            case State::Paused:
                if (idleTime < kIdleDurationToPause)
                    sleepOnly = false;
            }
        } catch (const std::runtime_error& err) {
            logError(err.what());
        }

        prevCheck = waitAndCheckTime(prevCheck, sleepOnly);
    }
}

void BreakTimer::timeBasedBackgroundWorker() 
{
    auto prevCheck = std::chrono::system_clock::now();
    while (state_ != State::Stopped)
        prevCheck = waitAndCheckTime(prevCheck);
}

std::chrono::system_clock::time_point BreakTimer::waitAndCheckTime(const std::chrono::system_clock::time_point prevPoint, const bool sleepOnly) 
{
    std::unique_lock<std::mutex> mLock(condVarMutex_);
    condVar_.wait_for(mLock, std::chrono::milliseconds(250));

    auto curPoint = std::chrono::system_clock::now();
    if (!sleepOnly) {
        if (state_ != State::Running)
            return curPoint;

        if ((curPoint - prevPoint) > timeLeft_.load()) {
            logInfo("Worker timeout");
            timeLeft_ = std::chrono::seconds(0);
            emit sigTimeout();
            pause();
        } else {
            timeLeft_ = timeLeft_.load() - (curPoint - prevPoint);
        }
    }
    return curPoint;
}

void BreakTimer::startProperWorker() 
{
    switch (type_) {
    case Type::Interactive:
        backgroundThread_ = std::thread(&BreakTimer::interactiveBackgroundWorker, this);
        break;
    case Type::TimeBased:
        backgroundThread_ = std::thread(&BreakTimer::timeBasedBackgroundWorker, this);
        break;
    default:
        throw std::runtime_error("startProperWorker switch error");
        exit(1);
    }
}
