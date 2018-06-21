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
    gameobject_t *object = (gameobject_t*)lua_tointeger(L, 0);
    std::cout << object << std::endl;
    float x = lua_tonumber(L, 1);
    std::cout << x << std::endl;
    float y = lua_tonumber(L, 2);
    std::cout << y << std::endl;
    float z = lua_tonumber(L, 3);
    std::cout << z << std::endl;
    object->pos += glm::vec3(x, y, z);
    std::cout << object->pos.x << std::endl;
    return 0;
}

int luaopen_gameobject(lua_State *L)
{
    lua_register(L, "gameobject_transform", gameobject_transform);
    return 0;
}

}

