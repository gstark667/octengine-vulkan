cmake_minimum_required(VERSION 3.9)

project(octengine)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g -Wall -DGLM_FORCE_RADIANS -DGLM_FORCE_DEPTH_ZERO_TO_ONE")
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

find_package(Vulkan)
find_package(glm)
find_package(OpenAL)
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
    include/audio.h
    include/gameobject.h
    include/buffer.h
    include/framebuffer.h
    include/descriptorset.h
    include/model.h
    include/camera.h
    include/light.h
    include/luaheaders.h
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
    src/audio.cpp
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

if(WIN32)
    set(EXTENSION ".dll")
ELSEIF(APPLE)
    set(EXTENSION ".dylib")
ELSE()
    set(EXTENSION ".so")
ENDIF()

SET(SHADERS "" PARENT_SCOPE)
file(GLOB vertshaders "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.vert")
file(GLOB fragshaders "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.frag")
foreach(item ${vertshaders})
    GET_FILENAME_COMPONENT(name ${item} NAME_WE)
    LIST(APPEND SHADERS "shaders/${name}_vert.spv")
    ADD_CUSTOM_COMMAND(
        OUTPUT "shaders/${name}_vert.spv"
        DEPENDS ${item}
        COMMAND glslangValidator -V ${item} -o "shaders/${name}_vert.spv"
    )
endforeach()
foreach(item ${fragshaders})
    GET_FILENAME_COMPONENT(name ${item} NAME_WE)
    LIST(APPEND SHADERS "shaders/${name}_frag.spv")
    ADD_CUSTOM_COMMAND(
        OUTPUT "shaders/${name}_frag.spv"
        DEPENDS ${item}
        COMMAND glslangValidator -V ${item} -o "shaders/${name}_frag.spv"
    )
endforeach()
SET(SHADERS ${SHADERS} PARENT_SCOPE)

ADD_CUSTOM_TARGET(
    shader_files
    DEPENDS ${SHADERS}
)

include_directories(${BULLET_INCLUDE_DIR})

ADD_LIBRARY(liboctengine SHARED ${SOURCES} ${HEADERS} ${SHADERS})
set_target_properties(liboctengine PROPERTIES OUTPUT_NAME "octengine")
target_link_libraries (liboctengine ${CMAKE_THREAD_LIBS_INIT} vulkan openal alut SDL2 assimp freeimage lua ${BULLET_LIBRARIES})

add_executable(mainoctengine src/main.cpp)
set_target_properties(mainoctengine PROPERTIES OUTPUT_NAME "octengine")
target_link_libraries (mainoctengine ${CMAKE_THREAD_LIBS_INIT} vulkan openal alut SDL2 assimp freeimage lua liboctengine ${BULLET_LIBRARIES})
target_include_directories (mainoctengine PUBLIC include/)
add_dependencies(mainoctengine shader_files)

SET(LUALIBS "" PARENT_SCOPE)
SET(BINARIES "" PARENT_SCOPE)
LIST(APPEND LUALIBS "liboctengine${EXTENSION}")
IF(WIN32)
LIST(APPEND BINARIES "octengine.exe")
ELSE()
LIST(APPEND BINARIES "octengine")
ENDIF()
LIST(APPEND BINARIES "mainoctengine")
LIST(APPEND BINARIES "liboctengine")
file(GLOB libfiles "${CMAKE_CURRENT_SOURCE_DIR}/lualibs/*.cpp")
foreach(item ${libfiles})
    GET_FILENAME_COMPONENT(item ${item} NAME_WE)
    LIST(APPEND LUALIBS "${item}${EXTENSION}")
    LIST(APPEND BINARIES "${item}")
    ADD_LIBRARY(${item} SHARED lualibs/${item}.cpp ${HEADERS})
    SET_TARGET_PROPERTIES(${item} PROPERTIES PREFIX "")
    TARGET_LINK_LIBRARIES(${item} lua liboctengine)
endforeach()
SET(LUALIBS ${LUALIBS} PARENT_SCOPE)
SET(BINARIES ${BINARIES} PARENT_SCOPE)
