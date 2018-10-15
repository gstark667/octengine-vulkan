cmake_minimum_required(VERSION 3.9)

project(octengine)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g -Wall -DGLM_FORCE_RADIANS -DGLM_FORCE_DEPTH_ZERO_TO_ONE")
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

find_package(Vulkan)
find_package(glm)
find_package(SDL2)
find_package(assimp)
find_package(Bullet)

include_directories(include/)

set(HEADERS
    include/util.h
    include/shader.h
    include/image.h
    include/texture.h
    include/physics.h
    include/pipeline.h
    include/scene.h
    include/gameobject.h
    include/buffer.h
    include/framebuffer.h
    include/descriptorset.h
    include/model.h
    include/camera.h
    include/light.h
    include/script.h
    include/settings.h
    include/application.h
)

set(SOURCES
    src/util.cpp
    src/shader.cpp
    src/image.cpp
    src/texture.cpp
    src/physics.cpp
    src/pipeline.cpp
    src/scene.cpp
    src/gameobject.cpp
    src/buffer.cpp
    src/framebuffer.cpp
    src/descriptorset.cpp
    src/model.cpp
    src/camera.cpp
    src/light.cpp
    src/script.cpp
    src/settings.cpp
    src/application.cpp
)

ADD_CUSTOM_COMMAND(
    OUTPUT shaders/
    COMMAND mkdir -p shaders
)

ADD_CUSTOM_COMMAND(
    OUTPUT shaders/frag.spv
    DEPENDS ${CMAKE_SOURCE_DIR}/shaders/shader.frag
    COMMAND glslangValidator -V ${CMAKE_SOURCE_DIR}/shaders/shader.frag -o shaders/frag.spv
)

ADD_CUSTOM_COMMAND(
    OUTPUT shaders/vert.spv
    DEPENDS ${CMAKE_SOURCE_DIR}/shaders/shader.vert
    COMMAND glslangValidator -V ${CMAKE_SOURCE_DIR}/shaders/shader.vert -o shaders/vert.spv
)

ADD_CUSTOM_COMMAND(
    OUTPUT shaders/offscreen_frag.spv
    DEPENDS ${CMAKE_SOURCE_DIR}/shaders/offscreen.frag
    COMMAND glslangValidator -V ${CMAKE_SOURCE_DIR}/shaders/offscreen.frag -o shaders/offscreen_frag.spv
)

ADD_CUSTOM_COMMAND(
    OUTPUT shaders/offscreen_vert.spv
    DEPENDS ${CMAKE_SOURCE_DIR}/shaders/offscreen.vert ${CMAKE_SOURCE_DIR}/shaders/rotate.vert
    COMMAND glslangValidator -V ${CMAKE_SOURCE_DIR}/shaders/offscreen.vert -o shaders/offscreen_vert.spv
)

ADD_CUSTOM_COMMAND(
    OUTPUT shaders/screen_frag.spv
    DEPENDS ${CMAKE_SOURCE_DIR}/shaders/screen.frag
    COMMAND glslangValidator -V ${CMAKE_SOURCE_DIR}/shaders/screen.frag -o shaders/screen_frag.spv
)

ADD_CUSTOM_COMMAND(
    OUTPUT shaders/screen_vert.spv
    DEPENDS ${CMAKE_SOURCE_DIR}/shaders/screen.vert
    COMMAND glslangValidator -V ${CMAKE_SOURCE_DIR}/shaders/screen.vert -o shaders/screen_vert.spv
)

ADD_CUSTOM_COMMAND(
    OUTPUT shaders/shadow_frag.spv
    DEPENDS ${CMAKE_SOURCE_DIR}/shaders/shadow.frag
    COMMAND glslangValidator -V ${CMAKE_SOURCE_DIR}/shaders/shadow.frag -o shaders/shadow_frag.spv
)

ADD_CUSTOM_COMMAND(
    OUTPUT shaders/shadow_vert.spv
    DEPENDS ${CMAKE_SOURCE_DIR}/shaders/shadow.vert ${CMAKE_SOURCE_DIR}/shaders/rotate.vert
    COMMAND glslangValidator -V ${CMAKE_SOURCE_DIR}/shaders/shadow.vert -o shaders/shadow_vert.spv
)

ADD_CUSTOM_COMMAND(
    OUTPUT shaders/sky_frag.spv
    DEPENDS ${CMAKE_SOURCE_DIR}/shaders/sky.frag
    COMMAND glslangValidator -V ${CMAKE_SOURCE_DIR}/shaders/sky.frag -o shaders/sky_frag.spv
)

ADD_CUSTOM_COMMAND(
    OUTPUT shaders/sky_vert.spv
    DEPENDS ${CMAKE_SOURCE_DIR}/shaders/sky.vert ${CMAKE_SOURCE_DIR}/shaders/rotate.vert
    COMMAND glslangValidator -V ${CMAKE_SOURCE_DIR}/shaders/sky.vert -o shaders/sky_vert.spv
)

ADD_CUSTOM_TARGET(
    shader_files
    DEPENDS shaders/ shaders/frag.spv shaders/vert.spv shaders/offscreen_frag.spv shaders/offscreen_vert.spv shaders/screen_vert.spv shaders/screen_frag.spv shaders/shadow_vert.spv shaders/shadow_frag.spv shaders/sky_vert.spv shaders/sky_frag.spv
)

include_directories(${BULLET_INCLUDE_DIR})

ADD_LIBRARY(liboctengine SHARED ${SOURCES} ${HEADERS} ${SHADERS})
set_target_properties(liboctengine PROPERTIES OUTPUT_NAME "octengine")

add_executable(mainoctengine src/main.cpp)
set_target_properties(mainoctengine PROPERTIES OUTPUT_NAME "octengine")
target_link_libraries (mainoctengine ${CMAKE_THREAD_LIBS_INIT} vulkan SDL2 assimp freeimage lua liboctengine ${BULLET_LIBRARIES})
target_include_directories (mainoctengine PUBLIC include/)
add_dependencies(mainoctengine shader_files)

ADD_LIBRARY(gameobject SHARED lualibs/gameobject.cpp ${HEADERS})
SET_TARGET_PROPERTIES(gameobject PROPERTIES PREFIX "")
TARGET_LINK_LIBRARIES(gameobject lua liboctengine)

ADD_LIBRARY(scene SHARED lualibs/scene.cpp ${HEADERS})
SET_TARGET_PROPERTIES(scene PROPERTIES PREFIX "")
TARGET_LINK_LIBRARIES(scene lua liboctengine)

ADD_LIBRARY(physics SHARED lualibs/physics.cpp ${HEADERS})
SET_TARGET_PROPERTIES(physics PROPERTIES PREFIX "")
TARGET_LINK_LIBRARIES(physics lua liboctengine)

ADD_LIBRARY(camera SHARED lualibs/camera.cpp ${HEADERS})
SET_TARGET_PROPERTIES(camera PROPERTIES PREFIX "")
TARGET_LINK_LIBRARIES(camera lua liboctengine)

ADD_LIBRARY(light SHARED lualibs/light.cpp ${HEADERS})
SET_TARGET_PROPERTIES(light PROPERTIES PREFIX "")
TARGET_LINK_LIBRARIES(light lua liboctengine)
