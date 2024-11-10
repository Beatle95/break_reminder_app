#include "Log.h"
#include "Functions.h"

namespace {

constexpr std::string_view kMainLogName("main.log");
constexpr std::string_view kOldLogName("old.log");
constexpr std::string_view kLogsDir("logs");
const int kLogMaxSize    = 1024 * 1024 * 2;
std::mutex mutex;

}

enum class LogMsgType {
    Info,
    Warning,
    Error
};

void log(const char *msg, LogMsgType type)
{
    std::lock_guard<std::mutex> lock(mutex);
    const auto logDir = getApplicationDataPath() / kLogsDir;
    const auto mainLogPath = logDir / kMainLogName;

    std::filesystem::create_directories(logDir);
    if (std::filesystem::exists(mainLogPath)) {
        const auto oldLogPath = logDir / kOldLogName;
        std::error_code err;
        auto mainLogSize = std::filesystem::file_size(mainLogPath, err);
        if (mainLogSize > kLogMaxSize) {
            if (std::filesystem::exists(oldLogPath))
                std::filesystem::remove(oldLogPath);
            std::filesystem::copy_file(mainLogPath, oldLogPath);
            std::filesystem::remove(mainLogPath);
        }
    }

    std::ofstream ofs;
    ofs.open(mainLogPath, std::ofstream::ate | std::ofstream::app);
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
