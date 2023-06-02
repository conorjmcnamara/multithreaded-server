#pragma once
#include <string>
#include <pthread.h>

enum class LogLevel {
    INFO,
    ERR
};

class Logger {
public:
    Logger(const std::string& logFile);
    void log(LogLevel level, const std::string& message);

private:
    const std::string logFile;
    pthread_mutex_t logMutex;

    std::string getTime();
    bool createLogFileIfNotExists(const std::string& filePath);
};