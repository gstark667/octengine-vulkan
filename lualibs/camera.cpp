extern "C"
{
#include <lua/lua.h>
#include <lua/lauxlib.h>
#include <lua/lualib.h>
}

#ifdef _WIN32
#define EXPORT __stdcall __declspec(dllexport)
#else
#define EXPORT
#endif

#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#include "camera.h"
#include "scene.h"


extern "C"
{

static int camera_get_gameobject(lua_State *L)
{
    camera_t *camera = (camera_t*)lua_tointeger(L, 1);
    lua_pushinteger(L, (lua_Integer)camera->object);
    return 1;
}

static int camera_set_fov(lua_State *L)
{
    camera_t *camera = (camera_t*)lua_tointeger(L, 1);
    camera->fov = (float)lua_tonumber(L, 2);
    camera_resize(camera);
    return 0;
}

static int camera_set_size(lua_State *L)
{
    camera_t *camera = (camera_t*)lua_tointeger(L, 1);
    camera->fov = -1.0f;
    camera->sizeX = (float)lua_tonumber(L, 2);
    camera->sizeY = (float)lua_tonumber(L, 3);
    camera_resize(camera);
    return 0;
}


int EXPORT luaopen_camera(lua_State *L)
{
    std::cout << "loading camera" << std::endl;
    lua_register(L, "camera_get_gameobject", camera_get_gameobject);
    lua_register(L, "camera_set_fov", camera_set_fov);
    lua_register(L, "camera_set_size", camera_set_size);
    return 0;
}

}

