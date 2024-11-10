#include "ConfigurationManager.h"
#include <optional>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include "Log.h"

const char *kWrongConfigErrorText = "Wrong configuration contents";
const char *kMode = "mode";
const char *kHour = "hour";
const char *kMin = "min";
const char *kSec = "sec";

void ConfigurationManager::writeToFile(const std::filesystem::path& pathToFile) 
{
    QJsonObject object;
    object[kMode] = static_cast<int>(mode_);
    object[kHour] = hour_;
    object[kMin] = minute_;
    object[kSec] = second_;

    QFile configFile(QString::fromStdString(pathToFile.u8string()));
    if (!configFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        throw std::runtime_error("Unable to open config file for writing");
    }
    configFile.write(QJsonDocument(object).toJson());
}

ConfigurationManager ConfigurationManager::readFromFile(const std::filesystem::path& pathToFile) 
{
    QFile configFile(QString::fromStdString(pathToFile.u8string()));
    if (!configFile.open(QIODevice::ReadOnly)) {
        throw std::runtime_error("Unable to open config file");
    }
    const auto jsonDocument = QJsonDocument::fromJson(configFile.readAll());
    
    if (!jsonDocument.isObject()) {
        throw std::runtime_error(kWrongConfigErrorText);
    }

    ConfigurationManager ret;
    const auto mainObj = jsonDocument.object();

    if (const auto val = mainObj[kMode].toVariant(); val.isValid()) {
        ret.mode_ = static_cast<Mode>(val.toInt());
    }
    if (const auto val = mainObj[kHour].toVariant(); val.isValid()) {
        ret.hour_ = val.toInt();
    }
    if (const auto val = mainObj[kMin].toVariant(); val.isValid()) {
        ret.minute_ = val.toInt();
    }
    if (const auto val = mainObj[kSec].toVariant(); val.isValid()) {
        ret.second_ = val.toInt();
    }

    return ret;
}
