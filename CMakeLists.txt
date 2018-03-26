cmake_minimum_required(VERSION 3.9)

project(octengine)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

find_package(glfw3)
find_package(glm)
find_package(assimp)

include_directories(include/)

set(HEADERS
    include/application.h
)

set(SOURCES
    src/main.cpp
    src/application.cpp
)

add_executable(octengine ${SOURCES} ${HEADERS})
target_link_libraries (octengine ${CMAKE_THREAD_LIBS_INIT})
target_include_directories (octengine PUBLIC include/)
