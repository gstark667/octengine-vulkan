extern "C"
{
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

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
    //pipeline_add_gameobject(scene, name);
    return 0;
}

int luaopen_scene(lua_State *L)
{
    lua_register(L, "scene_add_gameobject", scene_add_gameobject);
    return 0;
}

}

