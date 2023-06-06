#include "../include/server.h"
#include "../include/client.h"
#include "utils.cpp"
#include <memory>
#include <unistd.h>

void* startServer(void* serverPtr) {
    Server* server = static_cast<Server*>(serverPtr);
    server->start();
    return nullptr;
}

void* makeClientRequest(void* clientPtr) {
    Client* client = static_cast<Client*>(clientPtr);
    client->connectToServer();
    client->sendRequest("GET /index.html HTTP/1.1\r\nHost: 127.0.0.1\r\n\r\n");
    client->receiveResponse();
    client->closeConnection();
    return nullptr;
}

void simulateConcurrentRequests(sockaddr_in serverAddr, int numClients) {
    // create client objects across different ports
    std::vector<std::unique_ptr<Client>> clients;
    int clientPort = 9000;
    for (int i=0; i<numClients; ++i) {
        clients.push_back(std::make_unique<Client>(clientPort, serverAddr));
        clientPort++;
    }

    // send client requests
    std::vector<pthread_t> clientThreads;
    for (const auto& client : clients) {
        pthread_t clientThread;
        pthread_create(&clientThread, nullptr, makeClientRequest, client.get());
        clientThreads.push_back(clientThread);
    }
    
    // wait for all client threads to finish
    for (pthread_t clientThread : clientThreads) {
        pthread_join(clientThread, nullptr);
    }
}

int main() {
    std::unordered_map<std::string, std::string> envVariables = parseEnvFile(".env");
    std::string serverIP = envVariables["SERVER_IP"];
    int serverPort = envStrToInt(envVariables["SERVER_PORT"]);
    int maxThreads = envStrToInt(envVariables["MAX_THREADS"]);
    int cacheCapacity = envStrToInt(envVariables["CACHE_CAPACITY"]);

    Server server(serverIP, serverPort, maxThreads, cacheCapacity);
    pthread_t serverThread;
    pthread_create(&serverThread, nullptr, startServer, &server);

    usleep(1000);
    simulateConcurrentRequests(server.getServerAddr(), 5);

    // server.stop();
    pthread_join(serverThread, nullptr);
    return 0;
}