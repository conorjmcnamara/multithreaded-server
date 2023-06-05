#pragma once
#include "http_parser.h"
#include "http_response.h"
#include "logger.h"
#include <string>
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
    HttpParser http_parser;
    HttpResponse http_response;
    Logger logger;

    void initSocket();
    void initThreadPool();
    void addWorkerThread();
    static void* workerThreadRoutine(void* serverPtr);
    void removeWorkerThread();
    void cleanup();
    SOCKET acceptClientConnection();
    void enqueueClientRequest(SOCKET clientSocket);
    SOCKET dequeueClientRequest();
    void processClientRequest(SOCKET clientSocket);
    
};