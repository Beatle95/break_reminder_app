#include "PlatformEvents.h"

#ifdef _WIN32

PlatformEvents::PlatformEvents() noexcept
{
}

PlatformEvents::~PlatformEvents() noexcept
{
}

PlatformEvents& PlatformEvents::instance() 
{
    static PlatformEvents inst;
    return inst;
}

std::chrono::system_clock::duration PlatformEvents::getIdleTime() const
{
    LASTINPUTINFO plii;
    plii.cbSize = sizeof(PLASTINPUTINFO);
    if(!GetLastInputInfo(&plii))
        throw std::runtime_error("Unable to get Win32 last input info");

    return std::chrono::milliseconds(GetTickCount() - plii.dwTime);
}

#endif

// TODO: do for other platforms
