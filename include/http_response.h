#pragma once
#include "file_manager.h"
#include <string>
#include <unordered_set>
#include <unordered_map>

class HTTPResponse {
public:
    HTTPResponse(int cacheCapacity);
    std::string makeResponse(const std::string& request);

private:
    static const std::string supportedMethodsStr;
    static const std::unordered_set<std::string> supportedMethodsSet;
    static const std::unordered_map<std::string, std::string> mimeTypesMap;
    FileManager file_manager;

    std::string getMethodType(const std::string& request);
    std::string getFilePath(const std::string& request);
    std::string getContentType(const std::string& filePath);
    std::string makeSuccessResponse(int statusCode, const std::string& method, const std::string& contentType, const std::string& content);
    std::string makeErrorResponse(int statusCode, const std::string& statusMessage);
};