cmake_minimum_required(VERSION 3.15)

project(MultithreadedProxyServer)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(proxy_server
    proxy_server.cpp
    proxy_parse.cpp
    lru_cache.cpp
)

target_link_libraries(proxy_server ws2_32)