#include "../include/server.h"
#include <cstdio>
#include <cstdlib>
#include <format>
#include <string>

Server::Server(const std::string& serverIP, int serverPort, int maxThreads, int cacheCapacity)
    : serverIP(serverIP), serverPort(serverPort), maxThreads(maxThreads),
    http_response(cacheCapacity), logger("server.log") {}

void Server::start() {
    if (isRunning) {
        logger.log(LogLevel::ERR, "Server is already running");
        return;
    }

    isRunning = true;
    initSocket();
    initThreadPool();
    logger.log(LogLevel::INFO, "Server running on IP: " + serverIP + ", port: " + std::to_string(serverPort));

    while (isRunning) {
        SOCKET clientSocket = acceptClientConnection();
        if (clientSocket != INVALID_SOCKET) {
            enqueueClientRequest(clientSocket);
        }
    }
}

void Server::stop() {
    isRunning = false;
    cleanup();
}

sockaddr_in Server::getServerAddr() {
    return serverAddr;
}

void Server::initSocket() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        logger.log(LogLevel::ERR, "Failed to create server Winsock on port " + serverPort);
        return;
    }

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        logger.log(LogLevel::ERR, "Failed to create server socket on port " + serverPort);
        WSACleanup();
        return;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(serverPort);
    serverAddr.sin_addr.s_addr =  inet_addr(serverIP.c_str());
    
    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        logger.log(LogLevel::ERR, "Failed to bind server socket on port " + serverPort);
        stop();
        return;
    }

    // set the server's socket state to listen
    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        logger.log(LogLevel::ERR, "Failed to listen on server socket on port " + serverPort);
        stop();
        return;
    }
}

void Server::initThreadPool() {
    pthread_mutex_init(&mutex, nullptr);
    pthread_cond_init(&condition, nullptr);
    addWorkerThread();
}

void* Server::workerThreadRoutine(void* serverPtr) {
    Server* server = static_cast<Server*>(serverPtr);
    while (server->isRunning) {
        SOCKET clientSocket = server->dequeueClientRequest();
        if (clientSocket != INVALID_SOCKET) {
            server->processClientRequest(clientSocket);
            closesocket(clientSocket);
        }
    }
    return nullptr;
}

void Server::addWorkerThread() {
    pthread_t thread;
    pthread_create(&thread, nullptr, workerThreadRoutine, this);
    pthread_mutex_lock(&mutex);
    threadQueue.push(thread);
    pthread_mutex_unlock(&mutex);
}

void Server::removeWorkerThread() {
    pthread_t thread;
    pthread_mutex_lock(&mutex);
    if (!threadQueue.empty()) {
        thread = threadQueue.front();
        threadQueue.pop();
    }

    pthread_mutex_unlock(&mutex);
    if (thread != 0) {
        pthread_cancel(thread);
        pthread_detach(thread);
    }
}

SOCKET Server::acceptClientConnection() {
    return accept(serverSocket, nullptr, nullptr);
}

void Server::enqueueClientRequest(SOCKET clientSocket) {
    pthread_mutex_lock(&mutex);
    clientQueue.push(clientSocket);
    pthread_mutex_unlock(&mutex);

    // send a signal to wake up one of the waiting threads
    pthread_cond_signal(&condition);
    if (threadQueue.size() < maxThreads) {
        addWorkerThread();
    }
}

SOCKET Server::dequeueClientRequest() {
    SOCKET clientSocket = INVALID_SOCKET;
    pthread_mutex_lock(&mutex);
    while (clientQueue.empty() && isRunning) {
        // release the mutex lock and wait until signaled
        pthread_cond_wait(&condition, &mutex);
    }

    if (!clientQueue.empty()) {
        clientSocket = clientQueue.front();
        clientQueue.pop();
    }
    pthread_mutex_unlock(&mutex);
    return clientSocket;
}

void Server::processClientRequest(SOCKET clientSocket) {
    constexpr int bufferSize = 1024;
    std::vector<char> buffer(bufferSize, '\0');

    // get request data from the client socket
    int bytesRead = recv(clientSocket, buffer.data(), bufferSize - 1, 0);
    if (bytesRead == 0) {
        return;
    }

    std::string request(buffer.data());
    http_parser.printHeaders(request, pthread_self());
    logger.log(LogLevel::INFO, http_parser.getStartLine(request));

    std::string response = http_response.makeResponse(request);
    logger.log(LogLevel::INFO, http_parser.getStartLine(response));
    send(clientSocket, response.c_str(), response.size(), 0);
}

void Server::cleanup() {
    while (!threadQueue.empty()) {
        removeWorkerThread();
    }
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&condition);
    closesocket(serverSocket);
    WSACleanup();
}