extern "C"
{
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

#ifdef _WIN32
#define EXPORT __stdcall __declspec(dllexport)
#else
#define EXPORT
#endif

#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#include "light.h"
#include "scene.h"


extern "C"
{

static int light_get_camera(lua_State *L)
{
    light_t *light = (light_t*)lua_tointeger(L, 1);
    lua_pushinteger(L, (lua_Integer)&light->camera);
    return 1;
}

static int light_set_color(lua_State *L)
{
    light_t *light = (light_t*)lua_tointeger(L, 1);
    light->color.x = (float)lua_tonumber(L, 2);
    light->color.y = (float)lua_tonumber(L, 3);
    light->color.z = (float)lua_tonumber(L, 4);
    return 0;
}


int EXPORT luaopen_light(lua_State *L)
{
    lua_register(L, "light_get_camera", light_get_camera);
    lua_register(L, "light_set_color", light_set_color);
    return 0;
}

}

