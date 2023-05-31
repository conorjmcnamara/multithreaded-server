#include "file_handler.h"
#include <iostream>
#include <fstream>
#include <sstream>

const std::unordered_map<std::string, std::string> FileHandler::contentTypeMap = {
    { "html", "text/html" },
    { "css", "text/css" },
    { "js", "text/javascript" },
    { "png", "image/png" },
    { "jpg", "image/jpeg" },
    { "jpeg", "image/jpeg" },
    { "gif", "image/gif" }
};

std::string FileHandler::readFileWithResponse(const std::string& filePath) {
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        return generateErrorResponse(404, "not found");
    }

    std::ostringstream contentStream;
    contentStream << file.rdbuf();
    std::string fileContent = contentStream.str();
    file.close();

    std::string contentType = determineContentType(filePath);
    return generateSuccessResponse(200, fileContent, contentType);
}

std::string FileHandler::determineContentType(const std::string& filePath) {
    size_t dotIndex = filePath.find_last_of('.');
    if (dotIndex == std::string::npos) {
        // default if no dot extension is present
        return "text/plain";
    }

    std::string extension = filePath.substr(dotIndex + 1);
    auto iterator = contentTypeMap.find(extension);
    if (iterator != contentTypeMap.end()) {
        return iterator->second;
    }
    else {
        return "text/plain";
    }
}

std::string FileHandler::generateSuccessResponse(int statusCode, const std::string& content, const std::string& contentType) {
    std::ostringstream responseStream;
    responseStream << "HTTP/1.1 " << statusCode << " OK\r\n";
    responseStream << "Content-Length: " << content.size() << "\r\n";
    responseStream << "Content-Type: " << contentType << "\r\n\r\n";
    responseStream << content;
    return responseStream.str();
}

std::string FileHandler::generateErrorResponse(int statusCode, const std::string& statusMessage) {
    std::ostringstream responseStream;
    responseStream << "HTTP/1.1 " << statusCode << " " << statusMessage << "\r\n";
    responseStream << "Content-Type: text/html\r\n\r\n";
    responseStream << "<html><body><h1>" << statusCode << " " << statusMessage << "</h1></body></html>";
    return responseStream.str();
}