#pragma once
#include <string>

class ParseHeaders {
public:
    static void printHeaders(const std::string& request);
    
private:
    static std::string getRequest(const std::string& request);
    static std::string getHeaderField(const std::string& request, const std::string& fieldName);
};