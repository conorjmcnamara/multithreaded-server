#pragma once
#include <string>
#include <pthread.h>

class HTTPParser {
public:
    static void printHeaders(const std::string& request, pthread_t threadNum);
    
private:
    static std::string getStartLine(const std::string& request);
    static std::string getHeaderField(const std::string& request, const std::string& fieldName);
};