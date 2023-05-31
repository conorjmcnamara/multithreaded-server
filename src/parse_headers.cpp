#include "../include/parse_headers.h"
#include <sstream>
#include <iostream>

std::string ParseHeaders::getRequest(const std::string& request) {
    size_t endOfRequest = request.find("\r\n");
    if (endOfRequest != std::string::npos) {
        return request.substr(0, endOfRequest) + "\n";
    }
    return "";
}

std::string ParseHeaders::getHeaderField(const std::string& request, const std::string& fieldName) {
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

void ParseHeaders::printHeaders(const std::string& request) {
    std::stringstream outputStream;
    outputStream << "\n------------- Server -------------\n";
    outputStream << getRequest(request);
    outputStream << getHeaderField(request, "Host");
    outputStream << getHeaderField(request, "Connection");
    outputStream << getHeaderField(request, "User-Agent");
    outputStream << getHeaderField(request, "Referer");
    outputStream << getHeaderField(request, "Accept-Encoding");
    outputStream << getHeaderField(request, "Accept-Language");
    std::cout << outputStream.str();
}