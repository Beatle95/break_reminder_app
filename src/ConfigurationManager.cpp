#include "ConfigurationManager.h"
#include <stdexcept>
#include "Log.h"

void ConfigurationManager::writeToFile(const std::filesystem::path& pathToFile) 
{
    std::ofstream ofs;
    ofs.open(pathToFile, std::ofstream::trunc);
    if (!ofs.good())
        throw std::runtime_error("Unable to open config file");

    ofs << std::to_string(static_cast<int>(mode_)) << std::endl
        << std::to_string(hour_) << std::endl
        << std::to_string(minute_) << std::endl
        << std::to_string(second_) << std::endl;
}

ConfigurationManager ConfigurationManager::readFromFile(const std::filesystem::path& pathToFile) 
{
    std::ifstream ifs;
    ifs.open(pathToFile);
    if (!ifs.good())
        throw std::runtime_error("Unable to open config file, probably config is missing");

    ConfigurationManager ret;
    std::string line; 

    try {
        std::getline(ifs, line);
        ret.mode_ = static_cast<Mode>(std::stoi(line));
        std::getline(ifs, line);
        ret.hour_ = std::stoi(line);
        std::getline(ifs, line);
        ret.minute_ = std::stoi(line);
        std::getline(ifs, line);
        ret.second_ = std::stoi(line);
    } catch (const std::invalid_argument& err) {
        throw std::runtime_error("Unable to load configuration file");
    }

    return ret;
}
