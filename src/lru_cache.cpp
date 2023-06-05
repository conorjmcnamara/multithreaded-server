#include "../include/lru_cache.h"

LRUCache::LRUCache(int capacity)
    : capacity(capacity) {
    // head.next points to LRU, tail.prev points to MRU
    head = std::make_shared<Node>("", "");
    tail = std::make_shared<Node>("", "");
    head->next = tail;
    tail->prev = head;
    pthread_mutex_init(&cacheMutex, nullptr);
}

std::string LRUCache::get(const std::string& key) {
    pthread_mutex_lock(&cacheMutex);
    if (cache.find(key) != cache.end()) {
        std::shared_ptr<Node> node = cache[key];
        removeNode(node);
        moveToMRU(node);
        pthread_mutex_unlock(&cacheMutex);
        return node->value;
    }
    pthread_mutex_unlock(&cacheMutex);
    return "";
}

void LRUCache::put(const std::string& key, const std::string& value) {
    pthread_mutex_lock(&cacheMutex);
    if (cache.find(key) != cache.end()) {
        removeNode(cache[key]);
    }

    std::shared_ptr<Node> node = std::make_shared<Node>(key, value);
    cache[key] = node;
    moveToMRU(node);

    if (cache.size() > capacity) {
        std::shared_ptr<Node> lru = head->next;
        removeNode(lru);
        cache.erase(lru->key);
    }
    pthread_mutex_unlock(&cacheMutex);
}

void LRUCache::moveToMRU(std::shared_ptr<Node> node) {
    std::shared_ptr<Node> prevNode = tail->prev;
    prevNode->next = node;
    node->prev = prevNode;
    node->next = tail;
    tail->prev = node;
}

void LRUCache::removeNode(std::shared_ptr<Node> node) {
    std::shared_ptr<Node> prevNode = node->prev;
    std::shared_ptr<Node> nextNode = node->next;
    prevNode->next = nextNode;
    nextNode->prev = prevNode;
}