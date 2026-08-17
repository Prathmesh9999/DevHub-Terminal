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

#=====================================
#Configuration
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