#include "../include/http_response.h"
#include <sstream>

HTTPResponse::HTTPResponse(int cacheCapacity)
    : file_manager(cacheCapacity) {}

std::string HTTPResponse::makeResponse(const std::string& filePath, const std::string& method) {
    std::string content = file_manager.readFile(filePath);
    if (content.empty()) {
        return makeErrorResponse(404, "Not Found");
    }
    return makeSuccessResponse(200, method, content, file_manager.getContentType(filePath));
}

std::string HTTPResponse::makeSuccessResponse(int statusCode, const std::string& method, const std::string& content, const std::string& contentType) {
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
    stream << "Content-Type: text/html\r\n\r\n";
    stream << content.str();
    return stream.str();
}