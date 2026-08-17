cmake_minimum_required(VERSION 3.16)

project(distributed_kv)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(kv_server 
    src/main.cpp
    src/kv_store.cpp
    src/request.cpp
    src/handler.cpp
    src/thread_pool.cpp
    src/client_handler.cpp
    src/wal.cpp
)

target_link_libraries(kv_server 
    PRIVATE pthread
)