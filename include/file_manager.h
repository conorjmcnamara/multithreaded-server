#pragma once
#include "lru_cache.h"
#include <string>
#include <unordered_map>

class FileManager {
public:
    FileManager(int cacheCapacity);
    std::string readFileWithResponse(const std::string& filePath);

private:
    LRUCache cache;
    static const std::unordered_map<std::string, std::string> contentTypeMap;
    std::string getContentType(const std::string& filePath);
    std::string makeSuccessResponse(int statusCode, const std::string& content, const std::string& contentType);
    std::string makeErrorResponse(int statusCode, const std::string& statusMessage);
};