# Multi-threaded Web Server

A multi-threaded web server written in object-oriented C++ that uses TCP/IP sockets and POSIX threads to concurrently serve HTTP requests. Mutex locks protect simultaneous access of shared resources by ensuring only one thread has access at a time, which prevents race conditions. Condition variables allow threads to wait efficiently until a condition is met before proceeding, which involves the thread releasing its mutex lock and sleeping until another thread signals the condition.

The server runs on its own thread and when client requests arrive, the server dynamically creates new threads to process them concurrently up to a threshold ```maxThreads```. When this value is reached, incoming requests wait in a queue until one of the threads dequeues and processes it.

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
# start compiled program
$ cd build && ./multithreaded-server
```

```main.cpp``` simulates concurrent usage of the web server by instantiating and processing multiple client objects. Once the server is running, navigating to [http://127.0.0.1:8080/index.html](http://127.0.0.1:8080/index.html) in a web browser would also display the result of making a request to the server. The browser will initially request ```index.html```. Upon receipt, it will notice the file contains references to a stylesheet, script and image, and will send additional requests for these. 