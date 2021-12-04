#ifndef LOG
#define LOG
#include <ctime>
#include <fstream>
#include <filesystem>
#include <mutex>

void logWarning(const char *msg);
void logError(const char *msg);
void logInfo(const char *msg);

#endif
