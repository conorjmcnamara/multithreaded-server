#pragma once
#include <string>

class HttpParser {
public:
    static std::string getStartLine(const std::string& request);
    static std::string getHeaderFieldVal(const std::string& message, const std::string& field);
    static std::string getResponseCode(const std::string& response);
};