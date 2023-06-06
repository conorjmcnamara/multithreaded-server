# Multi-threaded Web Server

A multi-threaded web server written in object-oriented C++ that uses TCP/IP sockets and POSIX threads to concurrently serve HTTP requests, delivering 11,000 reqs/sec. GET, HEAD and POST are supported.

The server handles the HTTP protocol and serving static files. It also supports a microservice architecture for external API processes to act as high-level interfaces to the server, and these can implement handling for dynamic requests, such as POST. The APIs can be built with any language or architecture. For demonstration purposes, an API is implemented with Python Flask.

## Installation

### Prerequisites

- C++
- CMake
- Python

```bash
# start CMake build
$ cmake -B build && cmake --build build

# install external API dependencies
$ cd api && pip install -r requirements.txt
```

Modify ```.env``` to change IP addresses, ports, the number of threads and cache capacity.

### Running the Application

```bash
# start web server
$ cd build && ./multithreaded-server

# start external API
$ cd api && flask run
```

```bash
# GET request
$ curl -i http://127.0.0.1:8080/index.html

# HEAD request
$ curl -I http://127.0.0.1:8080/index.html

# POST request
$ curl -X POST -d "username=John%20Doe" -H "Content-Type: application/x-www-form-urlencoded" -i http://127.0.0.1:8080/api/user
```

```main.cpp``` simulates concurrent usage of the web server by sending requests from multiple client objects. Once the server is running, navigating to [http://127.0.0.1:8080/index.html](http://127.0.0.1:8080/index.html) in a web browser would display the result of making requests to the server. The browser will initially request ```index.html``` and upon receipt it will notice the file contains references to a stylesheet, script and image, and will send additional requests for these.

## Architecture

### Web Server and Microservice APIs

The web server binds to a given port on a given address and listens for incoming HTTP requests. Allowing external APIs to interface with the web server reduces its workload and improves scalability. These APIs can implement the high-level application-specific logic for certain routes needed for files stored on the server. Certain requests, such as POST, are dynamic and processing these may require interacting with databases or other services. 

As a demonstration, the server routes POST requests to a Python Flask API via Curl, and receives a result back. Note that the API can be implemented with any language or architecture so long as it communicates appropriately with the server.

### Multi-threading and Scheduling

The server runs on its own thread and when client requests arrive, the server pushes them to a queue and dynamically creates new worker threads to process them concurrently up to a threshold ```MAX_THREADS```. When this value is reached, incoming requests wait in the queue until one of the threads dequeues and processes it.

Mutex locks are used to prevent simultaneous access of shared resources by ensuring only one thread has access at a time. Condition variables are used to enable running threads to standby efficiently until some condition is met before proceeding, which involves the thread releasing its mutex lock and sleeping until signaled.

### Caching

A thread-safe Least Recently Used (LRU) cache is implemented with smart pointers, a hashmap and doubly linked list. It's used to store file paths with their respective content streams in memory to reduce repetitive I/O and thus latency. When the cache capacity is reached, the entry that is least recently used is removed.

### Logging

The server logs each client response with the following format:
> [IP] - [timestamp] [log level] [HTTP start line] [response code] [response size in bytes]

```bash
# example log
127.0.0.1:8080 - 2023-06-04 20:13:23 [INFO] "GET /index.html HTTP/1.1" 200 617
```

### Performance Testing

The web server was benchmarked with the Apache Bench command below. The server processed 11,000 reqs/sec:

```bash
# 1000 requests over 10 concurrent threads
$ ab -n 1000 -c 10 http://127.0.0.1:8080/index.html
```

```
This is ApacheBench, Version 2.3 <$Revision: 1903618 $>
Copyright 1996 Adam Twiss, Zeus Technology Ltd, http://www.zeustech.net/
Licensed to The Apache Software Foundation, http://www.apache.org/

Benchmarking 127.0.0.1 (be patient)
Completed 100 requests
Completed 200 requests
Completed 300 requests
Completed 400 requests
Completed 500 requests
Completed 600 requests
Completed 700 requests
Completed 800 requests
Completed 900 requests
Completed 1000 requests
Finished 1000 requests


Server Software:
Server Hostname:        127.0.0.1
Server Port:            8080

Document Path:          /index.html
Document Length:        604 bytes

Concurrency Level:      10
Time taken for tests:   0.090 seconds
Complete requests:      1000
Failed requests:        0
Total transferred:      669000 bytes
HTML transferred:       604000 bytes
Requests per second:    11156.11 [#/sec] (mean)
Time per request:       0.896 [ms] (mean)
Time per request:       0.090 [ms] (mean, across all concurrent requests)
Transfer rate:          7288.51 [Kbytes/sec] received

Connection Times (ms)
              min  mean[+/-sd] median   max
Connect:        0    0   0.2      0       1
Processing:     0    1   0.4      1       2
Waiting:        0    1   0.5      1       2
Total:          0    1   0.4      1       2

Percentage of the requests served within a certain time (ms)
  50%      1
  66%      1
  75%      1
  80%      1
  90%      1
  95%      1
  98%      1
  99%      2
 100%      2 (longest request)
```