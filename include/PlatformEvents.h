#ifndef __PLATFORMEVENTS_H__
#define __PLATFORMEVENTS_H__
#include <chrono>
#include <stdexcept>

class PlatformEvents {
public:
    PlatformEvents(const PlatformEvents& other) = delete;
    PlatformEvents& operator=(const PlatformEvents& other) = delete;
    static PlatformEvents& instance();

    std::chrono::system_clock::duration getIdleTime() const;
private:
    PlatformEvents() noexcept = default;
    ~PlatformEvents() noexcept = default;
};

#endif // __PLATFORMEVENTS_H__
