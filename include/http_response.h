#pragma once
#include "file_manager.h"
#include <string>
#include <unordered_set>
#include <unordered_map>

class HttpResponse {
public:
    HttpResponse(int cacheCapacity);
    std::string makeResponse(const std::string& request);

private:
    static const std::string supportedMethodsStr;
    static const std::unordered_set<std::string> supportedMethodsSet;
    static const std::unordered_map<std::string, std::string> mimeTypes;
    static const std::unordered_map<int, std::string> statusMessages;
    FileManager file_manager;

    std::string getHttpMethodType(const std::string& request);
    std::string getFilePath(const std::string& request);
    std::string getMimeType(const std::string& filePath);
    std::string makeSuccessResponse(int statusCode, const std::string& method, const std::string& contentType, const std::string& content);
    std::string makeErrorResponse(int statusCode);
};