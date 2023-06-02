#include "../include/logger.h"
#include <fstream>
#include <iostream>
#include <filesystem>
#include <sstream>
#include <iomanip>

Logger::Logger(const std::string& logFile)
    : logFile(logFile) {
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

std::string Logger::getTime() {
    std::time_t currentTime = std::time(nullptr);
    std::stringstream timeStream;
    timeStream << std::put_time(std::localtime(&currentTime), "%Y-%m-%d %H:%M:%S");
    return timeStream.str();
}

void Logger::log(LogLevel level, const std::string& message) {
    pthread_mutex_lock(&logMutex);
    std::string logLevelStr;
    switch (level) {
        case LogLevel::INFO:
            logLevelStr = "[INFO]";
            break;
        case LogLevel::ERR:
            logLevelStr = "[ERROR]";
            break;
    }

    std::string filePath = "../logs/" + logFile;
    if (!createLogFileIfNotExists(filePath)) {
        pthread_mutex_unlock(&logMutex);
        return;
    }

    std::ofstream logFileStream(filePath, std::ios::app);
    if (logFileStream.is_open()) {
        logFileStream << getTime() << " " << logLevelStr << " " << message << "\n";
        logFileStream.close();
    }
    else {
        std::cerr << "Failed to open log file at " << filePath << "\n";
    }
    pthread_mutex_unlock(&logMutex);
}