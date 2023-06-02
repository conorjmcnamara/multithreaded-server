#pragma once
#include "logger.h"
#include <string>
#include <windows.h>

class Client {
public:
    Client(int clientPort, sockaddr_in serverAddr);
    void connectToServer();
    void sendRequest(const std::string& request);
    void receiveResponse();
    void closeConnection();

private:
    int clientPort;
    sockaddr_in serverAddr;
    sockaddr_in clientAddr;
    SOCKET clientSocket;
    Logger logger;
};