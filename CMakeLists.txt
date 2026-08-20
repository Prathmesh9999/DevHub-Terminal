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

#====================================
# Core
#====================================
add_library(devhub_core
    core/router.c
    core/chat.c
)

target_include_directories(devhub_core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/core
        ${CMAKE_CURRENT_SOURCE_DIR}/networking
)

target_link_libraries(devhub_core
    PUBLIC
        devhub_networking
)

add_executable(router_test
    tests/router_test.c
)

target_link_libraries(router_test
    PRIVATE
        devhub_core
)

#=====================================
# Networking
#=====================================

add_library(devhub_networking
    networking/socket.c
    networking/protocol.c
)

target_include_directories(devhub_networking
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/networking
)

target_link_libraries(devhub_networking
    PUBLIC
        ws2_32
)
add_executable(networking_test
    tests/networking_test.c
)

target_link_libraries(networking_test
    PRIVATE
        devhub_networking
)

add_executable(server_socket_test
    tests/server_socket_test.c
)

target_link_libraries(server_socket_test
    PRIVATE
        devhub_core
)

add_executable(client_socket_test
    tests/client_socket_test.c
)

target_link_libraries(client_socket_test
    PRIVATE
        devhub_networking
)
add_executable(protocol_test
    tests/protocol_test.c
)

target_link_libraries(protocol_test
    PRIVATE
        devhub_networking
)
#=====================================
# Configuration
#=====================================
add_library(devhub_config
    common/Config.cpp
)

target_include_directories(devhub_config
    PUBLIC
       ${CMAKE_CURRENT_SOURCE_DIR}/common
    )


# ==========================================
# Logger Library
# ==========================================

add_library(devhub_logger
    logger/logger.c
)

target_include_directories(devhub_logger
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/logger
)

# ==========================================
# DevHub Server
# ==========================================

add_executable(devhub_server
    server/server_main.cpp
)

target_link_libraries(devhub_server
    PRIVATE
        devhub_logger
        devhub_config
)

# ==========================================
# DevHub Client
# ==========================================

add_executable(devhub_client
    client/client_main.cpp
)

target_link_libraries(devhub_client
    PRIVATE
        devhub_logger
        devhub_config
)