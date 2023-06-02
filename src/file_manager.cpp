#include "file_manager.h"
#include "lru_cache.h"
#include <fstream>
#include <sstream>

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