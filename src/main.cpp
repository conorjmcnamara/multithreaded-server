#include "../include/server.h"
#include "../include/client.h"
#include <memory>
#include <unistd.h>

void* startServer(void* serverPtr) {
    Server* server = static_cast<Server*>(serverPtr);
    server->start();
    return nullptr;
}

void* startClientRequest(void* clientPtr) {
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
        pthread_create(&clientThread, nullptr, startClientRequest, client.get());
        clientThreads.push_back(clientThread);
        usleep(1000);
    }
    
    // wait for all client threads to finish
    for (pthread_t clientThread : clientThreads) {
        pthread_join(clientThread, nullptr);
    }
}

int main() {
    std::string serverIP = "127.0.0.1";
    int serverPort = 8080;
    int maxThreads = 10;
    Server server(serverIP, serverPort, maxThreads);
    pthread_t serverThread;
    pthread_create(&serverThread, nullptr, startServer, &server);

    usleep(10000);
    simulateConcurrentRequests(server.getServerAddr(), 5);

    // server.stop();
    pthread_join(serverThread, nullptr);
    return 0;
}