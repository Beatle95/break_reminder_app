#include "Functions.h"
#include <QApplication>

std::filesystem::path getExecutableDirPath()
{
    return std::filesystem::u8path(qApp->applicationDirPath().toUtf8().data());
}
