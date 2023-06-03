#include "../include/http_parser.h"
#include <sstream>
#include <iostream>

std::string HTTPParser::getStartLine(const std::string& request) {
    size_t endStartLine = request.find("\r\n");
    if (endStartLine != std::string::npos) {
        return request.substr(0, endStartLine);
    }
    return "";
}

std::string HTTPParser::getHeaderField(const std::string& request, const std::string& fieldName) {
    size_t startHeaderField = request.find(fieldName + ": ");
    if (startHeaderField != std::string::npos) {
        size_t endHeaderField = request.find("\r\n", startHeaderField);

        if (endHeaderField != std::string::npos) {
            size_t startHeaderFieldVal = startHeaderField + fieldName.length() + 2;
            return request.substr(startHeaderFieldVal, endHeaderField - startHeaderFieldVal);
        }
    }
    return "";
}

std::string HTTPParser::getResponseCode(const std::string& response) {
    size_t startCode = response.find(' ') + 1;
    return response.substr(startCode, 3);
}