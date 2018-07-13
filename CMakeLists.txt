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
    include/script.h
    include/application.h
)

set(SOURCES
    src/util.cpp
    src/shader.cpp
    src/image.cpp
    src/texture.cpp
    src/pipeline.cpp
    src/model.cpp
    src/camera.cpp
    src/script.cpp
    src/application.cpp
)

set(SHADERS
    shaders/frag.spv
    shaders/vert.spv
)

ADD_CUSTOM_COMMAND(
    OUTPUT shaders
    COMMAND mkdir -p shaders
)

ADD_CUSTOM_COMMAND(
    OUTPUT shaders/frag.spv
    DEPENDS shaders
    COMMAND glslangValidator -V ${CMAKE_SOURCE_DIR}/shaders/shader.frag -o shaders/frag.spv
)

ADD_CUSTOM_COMMAND(
    OUTPUT shaders/vert.spv
    DEPENDS shaders
    COMMAND glslangValidator -V ${CMAKE_SOURCE_DIR}/shaders/shader.vert -o shaders/vert.spv
)

ADD_LIBRARY(liboctengine SHARED ${SOURCES} ${HEADERS} ${SHADERS})
set_target_properties(liboctengine PROPERTIES OUTPUT_NAME "octengine")

add_executable(mainoctengine src/main.cpp)
set_target_properties(mainoctengine PROPERTIES OUTPUT_NAME "octengine")
target_link_libraries (mainoctengine ${CMAKE_THREAD_LIBS_INIT} vulkan glfw assimp freeimage lua liboctengine)
target_include_directories (mainoctengine PUBLIC include/)

ADD_LIBRARY(gameobject SHARED lualibs/gameobject.cpp ${HEADERS})
SET_TARGET_PROPERTIES(gameobject PROPERTIES PREFIX "")
TARGET_LINK_LIBRARIES(gameobject lua)

ADD_LIBRARY(scene SHARED lualibs/scene.cpp ${HEADERS})
SET_TARGET_PROPERTIES(scene PROPERTIES PREFIX "")
TARGET_LINK_LIBRARIES(scene lua liboctengine)

