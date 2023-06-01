#pragma once
#include <string>
#include <unordered_map>
#include <memory>
#include <pthread.h>

struct Node {
    std::string key;
    std::string value;
    std::shared_ptr<Node> prev;
    std::shared_ptr<Node> next;

    Node(const std::string& key, const std::string& value)
        : key(key), value(value), prev(nullptr), next(nullptr) {}
};

class LRUCache {
public:
    LRUCache(int capacity);
    std::string get(const std::string& key);
    void put(const std::string& key, const std::string& value);

private:
    int capacity;
    std::unordered_map<std::string, std::shared_ptr<Node>> cache;
    std::shared_ptr<Node> head;
    std::shared_ptr<Node> tail;
    pthread_mutex_t cacheMutex;

    void moveToMRU(std::shared_ptr<Node> node);
    void removeNode(std::shared_ptr<Node> node);
};