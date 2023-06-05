#include "../include/http_parser.h"
#include <sstream>
#include <iostream>

const std::string HttpParser::supportedMethodsStr = "GET, HEAD, POST";
const std::unordered_set<std::string> HttpParser::supportedMethodsSet = {"GET", "HEAD", "POST"};

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

std::string HttpParser::getHttpMethod(const std::string& request) {
    size_t firstSpace = request.find(' ');
    if (firstSpace != std::string::npos) {
        std::string method = request.substr(0, firstSpace);
        if (supportedMethodsSet.find(method) != supportedMethodsSet.end()) {
            return method;
        }
    }
    return "";
}

std::string HttpParser::getFilePath(const std::string& request) {
    std::string endpoint = getEndpoint(request);
    if (endpoint.empty()) {
        return "";
    }
    return "../static" + endpoint;
}

std::string HttpParser::getEndpoint(const std::string& request) {
    size_t startEndpoint = request.find(' ');
    if (startEndpoint == std::string::npos) {
        return "";
    }
    size_t endEndpoint = request.find(' ', startEndpoint + 1);
    if (endEndpoint == std::string::npos) {
        return "";
    }
    return request.substr(startEndpoint + 1, endEndpoint - startEndpoint -1);
}

std::string HttpParser::getPayload(const std::string& request) {
    std::istringstream iss(request);
    std::string line;

    // skip headers
    while (std::getline(iss, line)) {
        if (line.empty() || line == "\r") {
            break;
        }
    }

    std::string payload;
    std::getline(iss, payload);
    return payload;
}

std::string HttpParser::getResponseCode(const std::string& response) {
    size_t startCode = response.find(' ') + 1;
    return response.substr(startCode, 3);
}