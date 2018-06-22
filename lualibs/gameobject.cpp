extern "C"
{
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#include "model.h"


extern "C"
{

static int gameobject_transform(lua_State *L)
{
    gameobject_t *object = (gameobject_t*)lua_tointeger(L, 1);
    float x = lua_tonumber(L, 2);
    float y = lua_tonumber(L, 3);
    float z = lua_tonumber(L, 4);
    object->pos += glm::vec3(x, y, z);
    return 0;
}

static int gameobject_rotate(lua_State *L)
{
    gameobject_t *object = (gameobject_t*)lua_tointeger(L, 1);
    float x = lua_tonumber(L, 2);
    float y = lua_tonumber(L, 3);
    float z = lua_tonumber(L, 4);
    object->rot += glm::vec3(x, y, z);
    return 0;
}

int luaopen_gameobject(lua_State *L)
{
    lua_register(L, "gameobject_transform", gameobject_transform);
    lua_register(L, "gameobject_rotate", gameobject_rotate);
    return 0;
}

}

