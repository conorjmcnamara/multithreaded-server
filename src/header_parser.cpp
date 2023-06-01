#include "../include/header_parser.h"
#include <sstream>
#include <iostream>

std::string HeaderParser::getRequest(const std::string& request) {
    size_t endOfRequest = request.find("\r\n");
    if (endOfRequest != std::string::npos) {
        return request.substr(0, endOfRequest) + "\n";
    }
    return "";
}

std::string HeaderParser::getHeaderField(const std::string& request, const std::string& fieldName) {
    std::string field = fieldName + ": ";
    size_t start = request.find(field);
    if (start != std::string::npos) {
        size_t end = request.find("\r\n", start);
        if (end != std::string::npos) {
            return request.substr(start, end - start) + "\n";
        }
    }
    return "";
}

void HeaderParser::printHeaders(const std::string& request, pthread_t threadNum) {
    std::ostringstream stream;
    stream << "\n------------- Server thread: " << threadNum << " -------------\n";
    stream << getRequest(request);
    stream << getHeaderField(request, "Host");
    stream << getHeaderField(request, "Connection");
    stream << getHeaderField(request, "User-Agent");
    stream << getHeaderField(request, "Referer");
    stream << getHeaderField(request, "Accept-Encoding");
    stream << getHeaderField(request, "Accept-Language");
    std::cout << stream.str();
}