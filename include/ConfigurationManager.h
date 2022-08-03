#ifndef CONFIGURATION_MANAGER
#define CONFIGURATION_MANAGER
#include <fstream>
#include <filesystem>
#include <stdexcept>
#include <string>

enum class Mode {
    TimeBased,
    Interactive
};

class ConfigurationManager {
public:
    inline void setMode(Mode mode) { mode_ = mode; }
    inline void setHour(int hour) { hour_ = hour; }
    inline void setMinute(int minute) { minute_ = minute; }
    inline void setSecond(int second) { second_ = second; }

    inline Mode getMode() { return mode_; }
    inline int getHour() { return hour_; }
    inline int getMinute() { return minute_; }
    inline int getSecond() { return second_; }

    void writeToFile(const std::filesystem::path& pathToFile);
    static ConfigurationManager readFromFile(const std::filesystem::path& pathToFile);

private:
    Mode mode_;
    int hour_;
    int minute_;
    int second_;
};

#endif
