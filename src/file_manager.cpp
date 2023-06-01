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

std::string FileManager::readFileWithResponse(const std::string& filePath) {
    std::string cachedContent = cache.get(filePath);
    if (!cachedContent.empty()) {
        return makeSuccessResponse(200, cachedContent, getContentType(filePath));
    }

    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        return makeErrorResponse(404, "not found");
    }

    std::ostringstream stream;
    stream << file.rdbuf();
    std::string fileContent = stream.str();
    file.close();

    cache.put(filePath, fileContent);
    std::string contentType = getContentType(filePath);
    return makeSuccessResponse(200, fileContent, contentType);
}

std::string FileManager::getContentType(const std::string& filePath) {
    size_t dotIndex = filePath.find_last_of('.');
    // default if a dot extension is not present
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

std::string FileManager::makeSuccessResponse(int statusCode, const std::string& content, const std::string& contentType) {
    std::ostringstream stream;
    stream << "HTTP/1.1 " << statusCode << " OK\r\n";
    stream << "Content-Length: " << content.size() << "\r\n";
    stream << "Content-Type: " << contentType << "\r\n\r\n";
    stream << content;
    return stream.str();
}

std::string FileManager::makeErrorResponse(int statusCode, const std::string& statusMessage) {
    std::ostringstream stream;
    stream << "HTTP/1.1 " << statusCode << " " << statusMessage << "\r\n";
    stream << "Content-Type: text/html\r\n\r\n";
    stream << "<html><body><h1>" << statusCode << " " << statusMessage << "</h1></body></html>";
    return stream.str();
}