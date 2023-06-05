#pragma once
#include <string>
#include <pthread.h>

enum class LogLevel {
    INFO,
    ERR
};

struct LogData {
    std::string ip;
    LogLevel level;
    std::string startLine;
    std::string responseCode;
    std::string responseSize;

    LogData(const std::string& ip, LogLevel level, const std::string& startLine,
            const std::string& responseCode, const std::string& responseSize)
        : ip(ip), level(level), startLine(startLine), responseCode(responseCode), responseSize(responseSize) {}
};

class Logger {
public:
    Logger(const std::string& logFile);
    void log(LogLevel level, const std::string& message);
    void log(LogData logData);
    
private:
    bool hasCreatedLogFile;
    const std::string filePath;
    pthread_mutex_t logMutex;

    bool createLogFileIfNotExists(const std::string& filePath);
    void writeLog(const std::string& logLine);
    std::string getTime();
    std::string getLogLevelStr(LogLevel level);
};