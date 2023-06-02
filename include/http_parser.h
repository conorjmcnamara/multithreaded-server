#pragma once
#include <string>
#include <pthread.h>

class HTTPParser {
public:
    static void printHeaders(const std::string& request, pthread_t threadNum);
    static std::string getStartLine(const std::string& request);
    
private:
    static std::string getHeaderField(const std::string& request, const std::string& fieldName);
};