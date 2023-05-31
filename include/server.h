#pragma once
#include "file_handler.h"
#include "parse_headers.h"
#include <queue>
#include <pthread.h>
#include <windows.h>

class Server {
public:
    Server(const std::string& serverIP, int serverPort, int maxThreads);
    void start();
    void stop();
    sockaddr_in getServerAddr();

private:
    std::string serverIP;
    int serverPort;
    int maxThreads;
    bool isRunning = false;
    sockaddr_in serverAddr;
    SOCKET serverSocket;
    std::queue<SOCKET> clientQueue;
    std::queue<pthread_t> threadQueue;
    pthread_mutex_t mutex;
    pthread_cond_t condition;
    FileHandler file_handler;
    ParseHeaders parse_headers;

    void initSocket();
    void initThreadPool();
    static void* workerThreadRoutine(void* serverPtr);
    void addWorkerThread();
    void removeWorkerThread();
    SOCKET acceptClientConnection();
    void enqueueClientRequest(SOCKET clientSocket);
    SOCKET dequeueClientRequest();
    void processClientRequest(SOCKET clientSocket);
    void cleanup();
};