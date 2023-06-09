#pragma once
#include <string>
#include <fstream>
#include <queue>
#include <pthread.h>

enum class LogLevel {
    DEBUG,
    INFO,
    WARNING,
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
    ~Logger();
    void log(LogLevel level, const std::string& message);
    void log(LogData logData);
    void cleanup();
    
private:
    bool hasCreatedLogFile;
    const std::string filePath;
    std::ofstream logFileStream;
    std::queue<std::string> logQueue;
    pthread_t logThread;
    pthread_mutex_t logMutex;
    pthread_cond_t logCondition;

    static void* logThreadRoutine(void* logger);
    std::string dequeueLogRequest();
    bool createLogFileIfNotExists(const std::string& filePath);
    void writeLog(const std::string& logLine);
    std::string getTime();
    std::string getLogLevelStr(LogLevel level);
};