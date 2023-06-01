#include "../include/http_parser.h"
#include <sstream>
#include <iostream>

std::string HTTPParser::getStartLine(const std::string& request) {
    size_t endStartLine = request.find("\r\n");
    if (endStartLine != std::string::npos) {
        return request.substr(0, endStartLine) + "\n";
    }
    return "";
}

std::string HTTPParser::getHeaderField(const std::string& request, const std::string& fieldName) {
    std::string field = fieldName + ": ";
    size_t startHeader = request.find(field);
    if (startHeader != std::string::npos) {
        size_t endHeader = request.find("\r\n", startHeader);

        if (endHeader != std::string::npos) {
            return request.substr(startHeader, endHeader - startHeader) + "\n";
        }
    }
    return "";
}

void HTTPParser::printHeaders(const std::string& request, pthread_t threadNum) {
    std::ostringstream stream;
    stream << "\n------------- Request on server thread " << threadNum << " -------------\n";
    stream << getStartLine(request);
    stream << getHeaderField(request, "Host");
    stream << getHeaderField(request, "Connection");
    stream << getHeaderField(request, "User-Agent");
    stream << getHeaderField(request, "Referer");
    stream << getHeaderField(request, "Accept-Encoding");
    stream << getHeaderField(request, "Accept-Language");
    std::cout << stream.str();
}