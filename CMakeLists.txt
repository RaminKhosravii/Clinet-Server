# CMakeLists.txt
cmake_minimum_required(VERSION 3.10)
project(P3)

set(CMAKE_CXX_STANDARD 11)

include_directories(include)

# Server run
add_executable(server src/server.cpp
        include/server.hpp)

# Client run
add_executable(client src/client.cpp
        include/client.hpp
)

if(WIN32)
    target_link_libraries(server ws2_32)
    target_link_libraries(client ws2_32)
endif()

set(CMAKE_CXX_STANDARD 11)  #To use C++11
set(CMAKE_CXX_STANDARD_REQUIRED ON)