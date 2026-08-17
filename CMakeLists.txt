cmake_minimum_required(VERSION 3.20)

project(DevHub
    VERSION 1.0
    LANGUAGES C CXX
)

set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

message(STATUS "Configuring DevHub v1.0")

# -------------------------------
# DevHub Server
# -------------------------------

add_executable(devhub_server
    server/server_main.cpp
)

# -------------------------------
# DevHub Client
# -------------------------------

add_executable(devhub_client
    client/client_main.cpp
)