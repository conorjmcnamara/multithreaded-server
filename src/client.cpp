#include "../include/client.h"
#include <vector>
#include <windows.h>

Client::Client(int clientPort, sockaddr_in serverAddr)
    : clientPort(clientPort), serverAddr(serverAddr), logger("client.log") {}

void Client::connectToServer() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        logger.log(LogLevel::ERR, "Failed to create client Winsock on port " + std::to_string(clientPort));
        return;
    }

    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        logger.log(LogLevel::ERR, "Failed to create client socket on port " + std::to_string(clientPort));
        WSACleanup();
        return;
    }

    clientAddr.sin_family = AF_INET;
    clientAddr.sin_port = htons(clientPort);
    clientAddr.sin_addr.s_addr = INADDR_ANY;
    
    if (bind(clientSocket, (sockaddr*)&clientAddr, sizeof(clientAddr)) == SOCKET_ERROR) {
        logger.log(LogLevel::ERR, "Failed to bind client socket on port " + std::to_string(clientPort));
        closeConnection();
        return;
    }
    
    if (connect(clientSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        logger.log(LogLevel::ERR, "Failed to connect client socket on port " + std::to_string(clientPort) + " to server");
        closeConnection();
        return;
    }
}

void Client::sendRequest(const std::string& request) {
    if (send(clientSocket, request.c_str(), static_cast<int>(request.size()), 0) == SOCKET_ERROR) {
        logger.log(LogLevel::ERR, "Failed to send request to server from client port " + std::to_string(clientPort));
    }
}

void Client::receiveResponse() {
    constexpr int bufferSize = 1024;
    std::vector<char> buffer(bufferSize, '\0');
    std::string response;
    int bytesRead;
    
    // get response data from the client socket
    do {
        bytesRead = recv(clientSocket, buffer.data(), bufferSize - 1, 0);
        if (bytesRead > 0) {
            response.append(buffer.data(), bytesRead);
        }
    } while (bytesRead > 0);
}

void Client::closeConnection() {
    closesocket(clientSocket);
    WSACleanup();
}