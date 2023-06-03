#pragma once
#include <string>

class HTTPParser {
public:
    static std::string getStartLine(const std::string& request);
    static std::string getHeaderField(const std::string& request, const std::string& fieldName);
    static std::string getResponseCode(const std::string& response);
};