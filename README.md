# Distributed Key-Value Storage System

A C++17 key-value storage server built from scratch using TCP sockets and multithreading.

## Current Features

- TCP server using POSIX sockets
- Custom text-based protocol
- GET, PUT, and DELETE operations
- Persistent client connections
- TCP request framing using newline delimiters
- Worker thread pool
- Concurrent client handling

## Protocol

### PUT

PUT key value

Response:

OK

### GET

GET key

Response:

VALUE value

or:

NOT_FOUND

### DELETE

DELETE key

Response:

DELETED

or:

NOT_FOUND

## Build

```bash
mkdir build
cd build
cmake ..
make