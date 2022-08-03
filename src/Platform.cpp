#include "Platform.h"

#ifdef WIN32
#include <Windows.h>

std::filesystem::path getExecutableDirPath()
{    
    static WCHAR path[MAX_PATH];
    if (GetModuleFileNameW(NULL, path, MAX_PATH) == 0)
        return std::filesystem::path();
    return std::filesystem::weakly_canonical(path).parent_path();
}

#endif // WIN32
