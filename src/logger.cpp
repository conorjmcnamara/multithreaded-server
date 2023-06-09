#include "../include/logger.h"
#include <iostream>
#include <sstream>
#include <filesystem>

Logger::Logger(const std::string& logFile)
    : filePath("../logs/" + logFile) {
    hasCreatedLogFile = createLogFileIfNotExists(filePath);
    if (hasCreatedLogFile) {
        logFileStream.open(filePath, std::ios::app);
        if (!logFileStream.is_open()) {
            std::cerr << "Failed to open log file at " << filePath << "\n";
        }
        pthread_mutex_init(&logMutex, nullptr);
        pthread_cond_init(&logCondition, nullptr);
        pthread_create(&logThread, nullptr, logThreadRoutine, this);
    }
}

Logger::~Logger() {
    cleanup();
}

bool Logger::createLogFileIfNotExists(const std::string& filePath) {
    std::filesystem::path logFilePath(filePath);
    if (std::filesystem::exists(logFilePath)) {
        return true;
    }
    std::filesystem::create_directories(logFilePath.parent_path());
    std::ofstream createFileStream(logFilePath);
    if (!createFileStream) {
        std::cerr << "Failed to create log file at " << filePath << "\n";
        return false;
    }
    return true;
}

void* Logger::logThreadRoutine(void* loggerPtr) {
    Logger* logger = static_cast<Logger*>(loggerPtr);
    while (true) {
        std::string logLine = logger->dequeueLogRequest();
        if (!logLine.empty()) {
            logger->writeLog(logLine);
        }
    }
    return nullptr;
}

std::string Logger::dequeueLogRequest() {
    std::string logLine;
    pthread_mutex_lock(&logMutex);
    while (logQueue.empty()) {
        pthread_cond_wait(&logCondition, &logMutex);
    }
    if (!logQueue.empty()) {
        logLine = logQueue.front();
        logQueue.pop();
    }
    pthread_mutex_unlock(&logMutex);
    return logLine;
}

void Logger::log(LogLevel level, const std::string& message) {
    if (hasCreatedLogFile) {
        std::string logLine = getTime() + " " + getLogLevelStr(level) + " " + message + "\n";
        pthread_mutex_lock(&logMutex);
        logQueue.push(logLine);
        pthread_mutex_unlock(&logMutex);
        pthread_cond_signal(&logCondition);
    }
}

void Logger::log(LogData logData) {
    if (hasCreatedLogFile) {
        std::string logLine = logData.ip + " - " + getTime() + " " + getLogLevelStr(logData.level) + " \"" +
                              logData.startLine + "\" " + logData.responseCode + " " + logData.responseSize + "\n";
        pthread_mutex_lock(&logMutex);
        logQueue.push(logLine);
        pthread_mutex_unlock(&logMutex);
        pthread_cond_signal(&logCondition);
    }
}

void Logger::writeLog(const std::string& logLine) {
    if (logFileStream.is_open()) {
        logFileStream << logLine;
        logFileStream.flush();
    }
    else {
        std::cerr << "Failed to open log file at " << filePath << "\n";
    }
}

std::string Logger::getTime() {
    std::time_t currentTime = std::time(nullptr);
    std::stringstream timeStream;
    timeStream << std::put_time(std::localtime(&currentTime), "%Y-%m-%d %H:%M:%S");
    return timeStream.str();
}

std::string Logger::getLogLevelStr(LogLevel level) {
    switch (level) {
        case LogLevel::DEBUG:
            return "[DEBUG]";
        case LogLevel::INFO:
            return "[INFO]";
        case LogLevel::WARNING:
            return "[WARNING]";
        case LogLevel::ERR:
            return "[ERROR]";
        default:
            return "";
    }
}

void Logger::cleanup() {
    pthread_join(logThread, nullptr);
    pthread_mutex_destroy(&logMutex);
    pthread_cond_destroy(&logCondition);
    if (logFileStream.is_open()) {
        logFileStream.close();
    }
}