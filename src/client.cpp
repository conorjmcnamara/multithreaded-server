#include "../include/client.h"
#include <iostream>
#include <vector>
#include <windows.h>

Client::Client(int clientPort, sockaddr_in serverAddr)
    : clientPort(clientPort), serverAddr(serverAddr) {}

void Client::connectToServer() {
    // initialize network environment
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "failed to create client Winsock on port " << clientPort << "\n";
        return;
    }

    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "failed to create client socket on port " << clientPort << "\n";
        WSACleanup();
        return;
    }

    clientAddr.sin_family = AF_INET;
    clientAddr.sin_port = htons(clientPort);
    clientAddr.sin_addr.s_addr = INADDR_ANY;
    
    if (bind(clientSocket, (sockaddr*)&clientAddr, sizeof(clientAddr)) == SOCKET_ERROR) {
        std::cerr << "failed to bind client socket on port " << clientPort << "\n";
        closeConnection();
        return;
    }
    
    if (connect(clientSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "failed to connect client socket on port " << clientPort << " to server\n";
        closeConnection();
        return;
    }
    std::cout << "successfully connected to server from client port " << clientPort << "\n";
}

void Client::sendRequest(const std::string& request) {
    std::cout << "sending request from client port: " << clientPort << "\n";
    if (send(clientSocket, request.c_str(), static_cast<int>(request.length()), 0) == SOCKET_ERROR) {
        std::cerr << "failed to send request to server from client port " << clientPort << "\n";
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
    std::cout << "received response from server on client port " << clientPort << ", size: "
              << response.size() << " bytes\n";
}

void Client::closeConnection() {
    closesocket(clientSocket);
    WSACleanup();
}