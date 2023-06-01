#pragma once
#include "file_manager.h"
#include <string>

class HTTPResponse {
public:
    HTTPResponse(int cacheCapacity);
    std::string makeResponse(const std::string& filePath, const std::string& method);

private:
    FileManager file_manager;
    std::string getContentType(const std::string& filePath);
    std::string makeSuccessResponse(int statusCode, const std::string& method, const std::string& content, const std::string& contentType);
    std::string makeErrorResponse(int statusCode, const std::string& statusMessage);
};