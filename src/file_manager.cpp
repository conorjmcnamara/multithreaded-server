#include "file_manager.h"
#include "lru_cache.h"
#include <iostream>
#include <fstream>
#include <sstream>

const std::unordered_map<std::string, std::string> FileManager::contentTypeMap = {
    { "html", "text/html" },
    { "css", "text/css" },
    { "js", "text/javascript" },
    { "png", "image/png" },
    { "jpg", "image/jpeg" },
    { "jpeg", "image/jpeg" },
    { "gif", "image/gif" }
};

FileManager::FileManager(int cacheCapacity)
    : cache(cacheCapacity) {}

std::string FileManager::readFile(const std::string& filePath) {
    std::string cachedContent = cache.get(filePath);
    if (!cachedContent.empty()) {
        return cachedContent;
    }

    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        return "";
    }

    std::ostringstream stream;
    stream << file.rdbuf();
    file.close();
    std::string content = stream.str();
    cache.put(filePath, content);
    return content;
}

std::string FileManager::getContentType(const std::string& filePath) {
    size_t dotIndex = filePath.find_last_of('.');
    // default when a dot extension is not present
    if (dotIndex == std::string::npos) {
        return "text/plain";
    }

    std::string extension = filePath.substr(dotIndex + 1);
    auto iterator = contentTypeMap.find(extension);
    if (iterator != contentTypeMap.end()) {
        return iterator->second;
    }
    else {
        return "text/plain";
    }
}