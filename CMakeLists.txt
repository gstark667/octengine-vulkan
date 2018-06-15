cmake_minimum_required(VERSION 3.9)

project(octengine)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g")
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

find_package(glfw3)
find_package(glm)
find_package(assimp)

include_directories(include/)

set(HEADERS
    include/util.h
    include/shader.h
    include/image.h
    include/texture.h
    include/pipeline.h
    include/model.h
    include/camera.h
    include/scene.h
    include/script.h
    include/application.h
)

set(SOURCES
    src/main.cpp
    src/util.cpp
    src/shader.cpp
    src/image.cpp
    src/texture.cpp
    src/pipeline.cpp
    src/model.cpp
    src/camera.cpp
    src/scene.cpp
    src/script.cpp
    src/application.cpp
)

add_executable(octengine ${SOURCES} ${HEADERS})
target_link_libraries (octengine ${CMAKE_THREAD_LIBS_INIT} vulkan glfw assimp freeimage lua)
target_include_directories (octengine PUBLIC include/)
