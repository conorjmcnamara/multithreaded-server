#include "../include/logger.h"
#include <fstream>
#include <iostream>
#include <filesystem>
#include <sstream>
#include <iomanip>

Logger::Logger(const std::string& logFile)
    : filePath("../logs/" + logFile) {
    hasCreatedLogFile = createLogFileIfNotExists(filePath);
    pthread_mutex_init(&logMutex, nullptr);
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

void Logger::log(LogLevel level, const std::string& message) {
    if (hasCreatedLogFile) {
        writeLog(getTime() + " " + getLogLevelStr(level) + " " + message);
    }
}

void Logger::log(LogData logData) {
    if (hasCreatedLogFile) {
        writeLog(logData.ip + " - " + getTime() + " " + getLogLevelStr(logData.level) + " \"" +
                 logData.startLine + "\" " + logData.responseCode + " " + logData.responseSize);
    }
}

std::string Logger::getLogLevelStr(LogLevel level) {
    switch (level) {
        case LogLevel::INFO:
            return "[INFO]";
        case LogLevel::ERR:
            return "[ERROR]";
        default:
            return "";
    }
}

std::string Logger::getTime() {
    std::time_t currentTime = std::time(nullptr);
    std::stringstream timeStream;
    timeStream << std::put_time(std::localtime(&currentTime), "%Y-%m-%d %H:%M:%S");
    return timeStream.str();
}

void Logger::writeLog(const std::string& logLine) {
    pthread_mutex_lock(&logMutex);
    std::ofstream logFileStream(filePath, std::ios::app);
    if (logFileStream.is_open()) {
        logFileStream << logLine << "\n";
        logFileStream.close();
    }
    else {
        std::cerr << "Failed to open log file at " << filePath << "\n";
    }
    pthread_mutex_unlock(&logMutex);
    std::cout << logLine << "\n";
}