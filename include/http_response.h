#pragma once
#include "file_manager.h"
#include "http_parser.h"
#include <string>
#include <unordered_map>

class HttpResponse {
public:
    HttpResponse(int cacheCapacity);
    std::string makeResponse(const std::string& request);

private:
    static const std::unordered_map<std::string, std::string> mimeTypes;
    static const std::unordered_map<int, std::string> statusMessages;
    std::string externalApiUrl;
    FileManager file_manager;
    HttpParser http_parser;

    std::string handleGetHead(const std::string& request, const std::string& method);
    std::string handlePost(const std::string& request);
    std::string callExternalApi(const std::string& request);
    static size_t writeCallback(void* receivedData, size_t size, size_t numElements, std::string* response);
    std::string getMimeType(const std::string& filePath);
    std::string makeSuccessResponse(int statusCode, const std::string& method, const std::string& contentType, const std::string& content);
    std::string makeErrorResponse(int statusCode);
};