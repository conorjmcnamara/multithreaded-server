#include "../include/http_parser.h"
#include <sstream>
#include <iostream>

std::string HttpParser::getStartLine(const std::string& request) {
    size_t endStartLine = request.find("\r\n");
    if (endStartLine != std::string::npos) {
        return request.substr(0, endStartLine);
    }
    return "";
}

std::string HttpParser::getHeaderFieldVal(const std::string& message, const std::string& field) {
    size_t startHeaderField = message.find(field + ": ");
    if (startHeaderField != std::string::npos) {
        size_t endHeaderField = message.find("\r\n", startHeaderField);

        if (endHeaderField != std::string::npos) {
            size_t startHeaderFieldVal = startHeaderField + field.length() + 2;
            return message.substr(startHeaderFieldVal, endHeaderField - startHeaderFieldVal);
        }
    }
    return "";
}

std::string HttpParser::getResponseCode(const std::string& response) {
    size_t startCode = response.find(' ') + 1;
    return response.substr(startCode, 3);
}