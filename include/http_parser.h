#pragma once
#include <string>
#include <unordered_set>

class HttpParser {
public:
    static const std::string supportedMethodsStr;

    static std::string getStartLine(const std::string& request);
    static std::string getHeaderFieldVal(const std::string& message, const std::string& field);
    static std::string getHttpMethod(const std::string& request);
    static std::string getFilePath(const std::string& request);
    static std::string getEndpoint(const std::string& request);
    static std::string getPayload(const std::string& request);
    static std::string getResponseCode(const std::string& response);

private:
    static const std::unordered_set<std::string> supportedMethodsSet;
};