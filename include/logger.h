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
    void log(LogLevel level, const std::string& ip, const std::string& startLine, const std::string& responseCode);
    
private:
    bool hasCreatedLogFile;
    const std::string filePath;
    pthread_mutex_t logMutex;

    bool createLogFileIfNotExists(const std::string& filePath);
    std::string getLogLevelStr(LogLevel level);
    std::string getTime();
    void writeLog(const std::string& logLine);
};