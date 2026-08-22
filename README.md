# Distributed Key-Value Storage System

A C++17 key-value storage server built from scratch using TCP sockets, multithreading, and persistent storage.


## Current Features

### Networking
- TCP server using POSIX sockets
- Custom text-based protocol
- GET, PUT, and DELETE operations
- Persistent client connections
- TCP request framing using newline delimiters
- Concurrent client handling

### Concurrency
- Worker thread pool
- Thread-safe task queue
- Concurrent access to the key-value store
- Reader/writer synchronization using `std::shared_mutex`

### Persistence & Recovery
- In-memory key-value index using `std::unordered_map`
- Write-ahead logging (WAL)
- Monotonically increasing WAL sequence numbers
- Periodic snapshots
- WAL compaction
- Snapshot + WAL-based recovery

### Testing
- Unit and integration tests
- Concurrent storage tests
- Concurrent TCP client tests
- Snapshot and recovery tests
- WAL and compaction tests
- Throughput benchmarking

## Protocol

### PUT

```
PUT key value
```

Response:

```
OK
```

### GET

```
GET key
```

Response:

```
VALUE value
```

or:

```
NOT_FOUND
```

### DELETE

```
DELETE key
```

Response:

```
DELETED
```

or:

```
NOT_FOUND
```

## Build

Requirements:

* C++17
* CMake
* POSIX-compatible operating system

```
mkdir build
cd build
cmake ..
make
```

## Run

Start the server:

```
./kv_server
```

The server listens on port `8080`.

## Run Tests

```
ctest --output-on-failure
```

Server-level concurrency tests require the server to be running separately.

## Future Work

* Multi-node cluster
* Key sharding and consistent hashing
* Data replication
* Failure detection and recovery
* Leader election / Raft consensus
* Cluster membership and rebalancing