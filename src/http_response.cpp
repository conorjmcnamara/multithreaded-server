#include "../include/http_response.h"
#include <sstream>
#include <regex>

HTTPResponse::HTTPResponse(int cacheCapacity)
    : file_manager(cacheCapacity) {}

const std::string HTTPResponse::supportedMethodsStr = "GET, HEAD";
const std::unordered_set<std::string> HTTPResponse::supportedMethodsSet = {"GET", "HEAD"};
const std::unordered_map<std::string, std::string> HTTPResponse::mimeTypesMap = {
    { "html", "text/html" },
    { "css", "text/css" },
    { "js", "text/javascript" },
    { "png", "image/png" },
    { "jpg", "image/jpeg" },
    { "jpeg", "image/jpeg" },
    { "gif", "image/gif" }
};

std::string HTTPResponse::getMethodType(const std::string& request) {
    size_t firstSpace = request.find(' ');
    if (firstSpace != std::string::npos) {
        std::string method = request.substr(0, firstSpace);
        if (supportedMethodsSet.find(method) != supportedMethodsSet.end()) {
            return method;
        }
    }
    return "";
}

std::string HTTPResponse:: getContentType(const std::string& filePath) {
    size_t dotIndex = filePath.find_last_of('.');
    // default when a dot extension is not present
    if (dotIndex == std::string::npos) {
        return "text/plain";
    }

    std::string extension = filePath.substr(dotIndex + 1);
    auto iterator = mimeTypesMap.find(extension);
    if (iterator != mimeTypesMap.end()) {
        return iterator->second;
    }
    else {
        return "text/plain";
    }
}

std::string HTTPResponse::getFilePath(const std::string& request) {
    size_t startFilePath = request.find(' ');
    size_t endFilePath = request.find(' ', startFilePath + 1);
    if (startFilePath != std::string::npos && endFilePath != std::string::npos) {
        return "../static/" + request.substr(startFilePath + 1, endFilePath - startFilePath - 1);
    }
    return "";
}

std::string HTTPResponse::makeResponse(const std::string& request) {
    std::string method = getMethodType(request);
    if (method.empty()) {
        return makeErrorResponse(405, "Method Not Allowed");
    }

    std::string filePath = getFilePath(request);
    if (filePath.empty()) {
        return makeErrorResponse(404, "Not Found");
    }

    std::string content = file_manager.readFile(filePath);
    if (content.empty()) {
        return makeErrorResponse(404, "Not Found");
    }
    return makeSuccessResponse(200, method, getContentType(filePath), content);
}

std::string HTTPResponse::makeSuccessResponse(int statusCode, const std::string& method, const std::string& contentType, const std::string& content) {
    std::ostringstream stream;
    stream << "HTTP/1.1 " << statusCode << " OK\r\n";
    stream << "Content-Length: " << content.size() << "\r\n";
    stream << "Content-Type: " << contentType << "\r\n\r\n";
    
    if (method == "GET") {
        stream << content;
    }
    return stream.str();
}

std::string HTTPResponse::makeErrorResponse(int statusCode, const std::string& statusMessage) {
    std::stringstream content;
    content << "<html><body><h1>" << statusCode << " " << statusMessage << "</h1></body></html>";

    std::ostringstream stream;
    stream << "HTTP/1.1 " << statusCode << " " << statusMessage << "\r\n";
    stream << "Content-Length: " << content.str().size() << "\r\n";
    stream << "Content-Type: text/html\r\n";

    if (statusCode == 405) {
        stream << "Allow: " << supportedMethodsStr << "\r\n";
    }

    stream << "\r\n" << content.str();
    return stream.str();
}