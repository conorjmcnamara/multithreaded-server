#pragma once
#include "file_manager.h"
#include "header_parser.h"
#include <queue>
#include <pthread.h>
#include <windows.h>

class Server {
public:
    Server(const std::string& serverIP, int serverPort, int maxThreads, int cacheCapacity);
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
    FileManager file_manager;
    HeaderParser header_parser;

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