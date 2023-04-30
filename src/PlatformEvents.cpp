#include "PlatformEvents.h"

PlatformEvents& PlatformEvents::instance() 
{
    static PlatformEvents inst;
    return inst;
}

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

std::chrono::system_clock::duration PlatformEvents::getIdleTime() const
{
    LASTINPUTINFO plii;
    plii.cbSize = sizeof(PLASTINPUTINFO);
    if(!GetLastInputInfo(&plii))
        throw std::runtime_error("Unable to get Win32 last input info");

    return std::chrono::milliseconds(GetTickCount() - plii.dwTime);
}

#elif __APPLE__

std::chrono::system_clock::duration PlatformEvents::getIdleTime() const
{
    // TODO: implementation for MAC OS
    return std::chrono::milliseconds(0);
}

#elif __linux__

std::chrono::system_clock::duration PlatformEvents::getIdleTime() const
{
    // TODO: implementation for Linux
    return std::chrono::milliseconds(0);
}

#endif // _WIN32


// TODO: do for other platforms
