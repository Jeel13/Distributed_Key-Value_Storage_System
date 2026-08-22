cmake_minimum_required(VERSION 3.16)

project(distributed_kv)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

add_executable(kv_server 
    src/main.cpp
    src/kv_store.cpp
    src/request.cpp
    src/handler.cpp
    src/thread_pool.cpp
    src/client_handler.cpp
    src/wal.cpp
    src/snapshot.cpp
)

add_executable(snapshot_test
    tests/snapshot_test.cpp
    src/snapshot.cpp
)

add_executable(concurrent_test
    tests/concurrent_test.cpp
    src/kv_store.cpp
    src/wal.cpp
    src/snapshot.cpp
)

add_executable(wal_compaction_test
    tests/wal_compaction_test.cpp
    src/wal.cpp
)

add_executable(recovery_test
    tests/recovery_test.cpp
    src/kv_store.cpp
    src/wal.cpp
    src/snapshot.cpp
)

add_executable(server_concurrency_test
    tests/server_concurrency_test.cpp
)

add_executable(benchmark
    tests/benchmark.cpp
)

target_link_libraries(kv_server 
    PRIVATE pthread
)

target_include_directories(snapshot_test PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

target_include_directories(concurrent_test PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

target_include_directories(wal_compaction_test PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

target_include_directories(recovery_test PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

add_test(NAME concurrent_test 
        COMMAND concurrent_test)

add_test(NAME wal_compaction_test 
        COMMAND wal_compaction_test)

add_test(NAME recovery_test
        COMMAND recovery_test)

add_test(NAME snapshot_test
        COMMAND snapshot_test)