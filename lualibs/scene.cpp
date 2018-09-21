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

#include "pipeline.h"


extern "C"
{

static int scene_add_gameobject(lua_State *L)
{
    pipeline_t *scene = (pipeline_t*)lua_tointeger(L, 1);
    const char *name = lua_tostring(L, 2);
    gameobject_t *object = pipeline_add_gameobject(scene, name);
    lua_pushinteger(L, (lua_Integer)object);
    return 1;
}

static int scene_add_script(lua_State *L)
{
    pipeline_t *scene = (pipeline_t*)lua_tointeger(L, 1);
    gameobject_t *object = (gameobject_t*)lua_tointeger(L, 2);
    const char *name = lua_tostring(L, 3);
    pipeline_add_script(scene, object, name);
    return 0;
}

static int scene_set_camera(lua_State *L)
{
    pipeline_t *scene = (pipeline_t*)lua_tointeger(L, 1);
    gameobject_t *object = (gameobject_t*)lua_tointeger(L, 2);
    scene->camera.object = object;
    return 0;
}

int EXPORT luaopen_scene(lua_State *L)
{
    lua_register(L, "scene_add_gameobject", scene_add_gameobject);
    lua_register(L, "scene_add_script", scene_add_script);
    lua_register(L, "scene_set_camera", scene_set_camera);
    return 0;
}

}

