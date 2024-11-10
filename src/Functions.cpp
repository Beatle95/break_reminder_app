#include "Functions.h"
#include <cassert>
#include <QApplication>
#include <QStandardPaths>
#include "version.h"

std::filesystem::path getExecutableDirPath()
{
    return std::filesystem::u8path(qApp->applicationDirPath().toUtf8().data());
}

std::filesystem::path getApplicationDataPath()
{
    auto appData = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    assert(appData.endsWith(PROJECT_NAME));
    return std::filesystem::u8path(appData.toStdString());
}
