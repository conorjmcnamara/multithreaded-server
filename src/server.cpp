#include "../include/server.h"
#include <iostream>
#include <cstdio>
#include <cstdlib>

std::regex Server::supportedMethods("^(GET|HEAD)");

Server::Server(const std::string& serverIP, int serverPort, int maxThreads, int cacheCapacity)
    : serverIP(serverIP), serverPort(serverPort), maxThreads(maxThreads), http_response(cacheCapacity) {}

void Server::start() {
    if (isRunning) {
        std::cout << "server is already running\n";
        return;
    }

    isRunning = true;
    initSocket();
    initThreadPool();
    std::cout << "server running on IP: " << serverIP << ", port: " << serverPort << "\n";

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
    // initialize network environment
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "failed to create server Winsock on port " << serverPort << "\n";
        return;
    }

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "failed to create server socket on port " << serverPort << "\n";
        WSACleanup();
        return;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(serverPort);
    serverAddr.sin_addr.s_addr =  inet_addr(serverIP.c_str());
    
    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "failed to bind server socket on port " << serverPort << "\n";
        stop();
        return;
    }

    // set the server's socket state to listen
    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "failed to listen on server socket on port " << serverPort << "\n";
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

std::string Server::getMethodType(const std::string& request) {
    std::smatch match;
    if (std::regex_search(request, match, supportedMethods)){
        return match.str(1);
    }
    return "";
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

    std::string method = getMethodType(request);
    if (method.empty()) {
        return;
    }

    size_t startFilePath = request.find(' ');
    size_t endFilePath = request.find(' ', startFilePath + 1);

    if (startFilePath != std::string::npos && endFilePath != std::string::npos) {
        std::string filePath = "../static/" + request.substr(startFilePath + 1, endFilePath - startFilePath - 1);
        std::string response = http_response.makeResponse(filePath, method);
        send(clientSocket, response.c_str(), response.size(), 0);
    }
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