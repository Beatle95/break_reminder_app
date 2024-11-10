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
    const auto log_dir = getApplicationDataPath() / kLogsDir;
    const auto main_log_path = log_dir / kMainLogName;

    std::filesystem::create_directories(log_dir);
    if (std::filesystem::exists(main_log_path)) {
        const auto old_log_path = log_dir / kOldLogName;
        std::error_code err;
        auto mainLogSize = std::filesystem::file_size(main_log_path, err);
        if (mainLogSize > kLogMaxSize) {
            if (std::filesystem::exists(old_log_path))
                std::filesystem::remove(old_log_path);
            std::filesystem::copy_file(main_log_path, old_log_path);
            std::filesystem::remove(main_log_path);
        }
    }

    std::ofstream ofs;
    ofs.open(main_log_path, std::ofstream::ate | std::ofstream::app);
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
