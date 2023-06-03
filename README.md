# Multi-threaded Web Server

A multi-threaded web server written in object-oriented C++ that uses TCP/IP sockets and POSIX threads to concurrently serve HTTP requests. GET and HEAD requests are supported. The server uses mutex locks and condition variables for synchronization, and an LRU cache to reduce latency.

## Installation
### Prerequisites
- C++
- CMake

```bash
# start CMake build
$ cmake -B build && cmake --build build
```

### Running the Application
```bash
# start application
$ cd build && ./multithreaded-server

# GET request
$ curl -i http://127.0.0.1:8080/index.html

# HEAD request
$ curl -I http://127.0.0.1:8080/index.html

# DELETE request (405 method not allowed)
$ curl -X DELETE -i http://127.0.0.1:8080/
```

```main.cpp``` simulates concurrent usage of the web server by instantiating and processing multiple client objects. Once the server is running, navigating to [http://127.0.0.1:8080/index.html](http://127.0.0.1:8080/index.html) in a web browser would display the result of making requests to the server. The browser will initially request ```index.html``` and upon receipt it will notice the file contains references to a stylesheet, script and image, and will send additional requests for these.

## Architecture

### Multi-threading

The server runs on its own thread and when client requests arrive, the server dynamically creates new threads to process them concurrently up to a threshold ```maxThreads```. When this value is reached, incoming requests wait in a queue until one of the threads dequeues and processes it.

Mutex locks protect simultaneous access of shared resources by ensuring only one thread has access at a time, which prevents race conditions. Condition variables allow threads to wait efficiently until a condition is met before proceeding, which involves the thread releasing its mutex lock and sleeping until another thread signals the condition.

### Caching

A thread and memory-safe Least Recently Used (LRU) cache is implemented with smart pointers, a hashmap and doubly linked list. It's used to store file paths with their respective content streams in memory for fast lookup. When the cache capacity is reached, the entry that is least recently used is removed.

### Logging

```bash
# log form
[timestamp] [log level] [IP] [HTTP start line] [response code]

# example log
2023-06-03 18:44:20 [INFO] 127.0.0.1:8080 "GET /index.html HTTP/1.1" 200
```