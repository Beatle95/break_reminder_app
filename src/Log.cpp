#include "Log.h"

static const char *kMainLogName = "main.log";
static const char *kOldLogName  = "old.log";
static const int kLogMaxSize    = 1024 * 1024 * 2;
static std::mutex mutex;

enum class LogMsgType {
    Info,
    Warning,
    Error
};

void log(const char *msg, LogMsgType type)
{
    std::lock_guard<std::mutex> lock(mutex);
    if (std::filesystem::exists(kMainLogName)) {
        std::error_code err;
        auto mainLogSize = std::filesystem::file_size(kMainLogName, err);
        if (mainLogSize > kLogMaxSize) {
            if (std::filesystem::exists(kOldLogName))
                std::filesystem::remove(kOldLogName);
            std::filesystem::copy_file(kMainLogName, kOldLogName);
            std::filesystem::remove(kMainLogName);
        }
    }

    std::ofstream ofs;
    ofs.open(kMainLogName, std::ofstream::ate | std::ofstream::app);
    auto ttime = std::time(0);
    auto localTime = std::localtime(&ttime);
    ofs << static_cast<int>(type) 
        << " "
        << std::put_time(localTime, "%Y-%m-%d %H:%M:%S") 
        << "    " 
        << msg
        << '\n';
}

void logInfo(const char *msg) 
{
    log(msg, LogMsgType::Info);
}

void logWarning(const char *msg) 
{
    log(msg, LogMsgType::Warning);
}

void logError(const char *msg) 
{
    log(msg, LogMsgType::Error);
}
