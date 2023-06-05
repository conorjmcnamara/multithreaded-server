#include "../include/http_response.h"
#include "utils.cpp"
#include <sstream>
#include <iostream>
#include <curl/curl.h>

const std::unordered_map<std::string, std::string> HttpResponse::mimeTypes = {
    { "html", "text/html" },
    { "css", "text/css" },
    { "js", "text/javascript" },
    { "png", "image/png" },
    { "jpg", "image/jpeg" },
    { "jpeg", "image/jpeg" },
    { "gif", "image/gif" }
};

const std::unordered_map<int, std::string> HttpResponse::statusMessages {
    {200, "OK"},
    {404, "Not Found"},
    {405, "Method Not Allowed"}
};

HttpResponse::HttpResponse(int cacheCapacity)
    : file_manager(cacheCapacity) {
    std::unordered_map<std::string, std::string> envVariables = parseEnvFile(".env");
    externalApiUrl = envVariables["API_IP"] + ":" + envVariables["API_PORT"];
}

std::string HttpResponse::makeResponse(const std::string& request) {
    std::string method = http_parser.getHttpMethod(request);
    if (method.empty()) {
        return makeErrorResponse(405);
    }
    if (method == "POST") {
        return handlePost(request);
    }
    return handleGetHead(request, method);
}

std::string HttpResponse::handleGetHead(const std::string& request, const std::string& method) {
    std::string filePath = http_parser.getFilePath(request);
    if (filePath.empty()) {
        return makeErrorResponse(404);
    }
    std::string content = file_manager.readFile(filePath);
    if (content.empty()) {
        return makeErrorResponse(404);
    }
    return makeSuccessResponse(200, method, getMimeType(filePath), content);
}

std::string HttpResponse::handlePost(const std::string& request) {
    std::string apiResponse = callExternalApi(request);
    if (apiResponse.empty()) {
        return makeErrorResponse(404);
    }
    return makeSuccessResponse(200, "POST", "application/json", apiResponse);
}

std::string HttpResponse::callExternalApi(const std::string& request) {
    CURL* curl = curl_easy_init();
    std::string url = externalApiUrl + http_parser.getEndpoint(request);
    std::string response;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    std::string method = http_parser.getHttpMethod(request);
    std::string payload = http_parser.getPayload(request);
    if (method == "POST") {
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, payload.size());
    }

    CURLcode result = curl_easy_perform(curl);
    if (result != CURLE_OK) {
        std::cerr << "Failed to call the external API: " << curl_easy_strerror(result) << "\n";
        return "";
    }

    curl_easy_cleanup(curl);
    return response;
}

size_t HttpResponse::writeCallback(void* receivedData, size_t size, size_t numElements, std::string* response) {
    size_t totalSize = size * numElements;
    response->append(static_cast<char*>(receivedData), totalSize);
    return totalSize;
}

std::string HttpResponse::getMimeType(const std::string& filePath) {
    size_t dotIndex = filePath.find_last_of('.');
    if (dotIndex == std::string::npos) {
        // default when a dot extension is not present
        return "text/plain";
    }

    std::string extension = filePath.substr(dotIndex + 1);
    auto iterator = mimeTypes.find(extension);
    if (iterator != mimeTypes.end()) {
        return iterator->second;
    }
    else {
        return "text/plain";
    }
}

std::string HttpResponse::makeSuccessResponse(int statusCode, const std::string& method, const std::string& contentType, const std::string& content) {
    std::ostringstream stream;
    stream << "HTTP/1.1 " << statusCode << " " << statusMessages.at(statusCode) << "\r\n";
    stream << "Content-Length: " << content.size() << "\r\n";
    stream << "Content-Type: " << contentType << "\r\n\r\n";
    if (method != "HEAD") {
        stream << content;
    }
    return stream.str();
}

std::string HttpResponse::makeErrorResponse(int statusCode) {
    std::stringstream content;
    content << "<html><body><h1>" << statusCode << " " << statusMessages.at(statusCode) << "</h1></body></html>";

    std::ostringstream stream;
    stream << "HTTP/1.1 " << statusCode << " " << statusMessages.at(statusCode) << "\r\n";
    stream << "Content-Length: " << content.str().size() << "\r\n";
    stream << "Content-Type: text/html\r\n";

    if (statusCode == 405) {
        stream << "Allow: " << http_parser.supportedMethodsStr << "\r\n";
    }

    stream << "\r\n" << content.str();
    return stream.str();
}