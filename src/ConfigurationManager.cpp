#include "ConfigurationManager.h"

void ConfigurationManager::writeToFile(const std::string& pathToFile) 
{
    std::ofstream ofs;
    ofs.open(pathToFile, std::ofstream::trunc);
    if (!ofs.good())
        throw std::runtime_error("Unable to open config file");

    ofs << std::to_string(static_cast<int>(mode_)) << "\n"
        << std::to_string(hour_) << "\n"
        << std::to_string(minute_) << "\n"
        << std::to_string(second_) << "\n";
}

ConfigurationManager ConfigurationManager::readFromFile(const std::string& pathToFile) 
{
    std::ifstream ifs;
    ifs.open(pathToFile);
    if (!ifs.good())
        throw std::runtime_error("Unable to open config file");

    ConfigurationManager ret;
    std::string line; 

    std::getline(ifs, line);
    ret.mode_ = static_cast<Mode>(std::stoi(line));
    std::getline(ifs, line);
    ret.hour_ = std::stoi(line);
    std::getline(ifs, line);
    ret.minute_ = std::stoi(line);
    std::getline(ifs, line);
    ret.second_ = std::stoi(line);

    return ret;
}
