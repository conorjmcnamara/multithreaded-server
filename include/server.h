#pragma once
#include "http_parser.h"
#include "http_response.h"
#include "logger.h"
#include <string>
#include <queue>
#include <pthread.h>
#include <windows.h>
#include <regex>

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
    HTTPParser http_parser;
    HTTPResponse http_response;
    Logger logger;
    static std::regex supportedMethods;

    void initSocket();
    void initThreadPool();
    static void* workerThreadRoutine(void* serverPtr);
    void addWorkerThread();
    void removeWorkerThread();
    SOCKET acceptClientConnection();
    void enqueueClientRequest(SOCKET clientSocket);
    SOCKET dequeueClientRequest();
    std::string getMethodType(const std::string& request);
    void processClientRequest(SOCKET clientSocket);
    void cleanup();
};