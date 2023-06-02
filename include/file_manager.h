#pragma once
#include "lru_cache.h"
#include <string>

class FileManager {
public:
    FileManager(int cacheCapacity);
    std::string readFile(const std::string& filePath);

private:
    LRUCache cache;
};