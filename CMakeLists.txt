cmake_minimum_required(VERSION 3.16)
project(streamver C)

set(CMAKE_C_STANDARD 11)

include_directories(.)
include_directories(server)
include_directories(text-client)

# Changing server output directory
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../server/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../server/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../server/bin)

# Server executable
add_executable(server.exe server/main.c server/estructuras.h)

# Changing server output directory
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../text-client/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../text-client/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../text-client/bin)

# Text Client executable
add_executable(text_client.exe text-client/main.c)
