#pragma once
#include "lru_cache.h"
#include <string>
#include <unordered_map>

class FileManager {
public:
    FileManager(int cacheCapacity);
    std::string readFile(const std::string& filePath);
    std::string getContentType(const std::string& filePath);

private:
    LRUCache cache;
    static const std::unordered_map<std::string, std::string> contentTypeMap;
};