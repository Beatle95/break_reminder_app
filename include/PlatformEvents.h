#ifdef _WIN32
#ifndef WINDOWS_HOOKS
#define WINDOWS_HOOKS
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <chrono>
#include <stdexcept>

class PlatformEvents {
public:
    PlatformEvents(const PlatformEvents& other) = delete;
    PlatformEvents& operator=(const PlatformEvents& other) = delete;
    static PlatformEvents& instance();

    std::chrono::system_clock::duration getIdleTime() const;
private:
    PlatformEvents() noexcept;
    ~PlatformEvents() noexcept;
};

#endif
#endif
