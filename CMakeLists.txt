cmake_minimum_required(VERSION 3.10)
project(ClientServer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Specify the include directories for headers
include_directories(include)

# Add Executable for the Client
add_executable(client src/main_client.cpp src/Client.cpp)

# Add Executable for the Server
add_executable(server src/main_server.cpp src/Server.cpp)