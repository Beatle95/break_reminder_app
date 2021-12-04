#ifndef BREAK_REMINDER
#define BREAK_REMINDER
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <stdexcept>
#include <thread>
#include <QObject>
#include "Log.h"
#include "PlatformEvents.h"

class BreakTimer : public QObject {
Q_OBJECT
public:
    enum class Type {
        Interactive,
        TimeBased
    };

    enum class State {
        Stopped,
        Paused,
        Running
    };

    explicit BreakTimer(Type type, int hour, int min, int sec);
    ~BreakTimer();
    BreakTimer() = delete;
    BreakTimer(const BreakTimer& other) = delete;

    void restart(const bool noLog = false);
    void pause(const bool noLog = false);
    void resume(const bool noLog = false);
    inline State getState() { return state_; }
    std::chrono::system_clock::duration getTimeLeft();

signals:
    void sigTimeout();

private:
    Type type_ = Type::Interactive;
    std::atomic<State>      state_;
    std::condition_variable condVar_;
    std::mutex              condVarMutex_;

    std::atomic<std::chrono::system_clock::duration> timeLeft_;
    std::chrono::system_clock::duration setupDuration_;

    std::thread backgroundThread_;
    void interactiveBackgroundWorker();
    void timeBasedBackgroundWorker();
    std::chrono::system_clock::time_point waitAndCheckTime(const std::chrono::system_clock::time_point prevPoint, const bool sleepOnly = false);
    void startProperWorker();
};

#endif
