#pragma once
#include <string>
#include <unordered_map>

class FileHandler {
public:
    std::string readFileWithResponse(const std::string& filePath);

private:
    static const std::unordered_map<std::string, std::string> contentTypeMap;
    std::string determineContentType(const std::string& filePath);
    std::string generateSuccessResponse(int statusCode, const std::string& content, const std::string& contentType);
    std::string generateErrorResponse(int statusCode, const std::string& statusMessage);
};