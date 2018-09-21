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

#include "gameobject.h"


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

static int gameobject_set_transform(lua_State *L)
{
    gameobject_t *object = (gameobject_t*)lua_tointeger(L, 1);
    float x = lua_tonumber(L, 2);
    float y = lua_tonumber(L, 3);
    float z = lua_tonumber(L, 4);
    object->pos = glm::vec3(x, y, z);
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

static int gameobject_set_rotation(lua_State *L)
{
    gameobject_t *object = (gameobject_t*)lua_tointeger(L, 1);
    float x = lua_tonumber(L, 2);
    float y = lua_tonumber(L, 3);
    float z = lua_tonumber(L, 4);
    object->rot = glm::vec3(x, y, z);
    return 0;
}

static int gameobject_scale(lua_State *L)
{
    gameobject_t *object = (gameobject_t*)lua_tointeger(L, 1);
    float scale = lua_tonumber(L, 2);
    object->scale *= scale;
    return 0;
}

static int gameobject_set_scale(lua_State *L)
{
    gameobject_t *object = (gameobject_t*)lua_tointeger(L, 1);
    float scale = lua_tonumber(L, 2);
    object->scale = scale;
    return 0;
}


static int gameobject_set_parent(lua_State *L)
{
    gameobject_t *object = (gameobject_t*)lua_tointeger(L, 1);
    gameobject_t *parent = (gameobject_t*)lua_tointeger(L, 2);
    object->parent = (gameobject_t*)parent;
    return 0;
}

static int gameobject_get_parent(lua_State *L)
{
    gameobject_t *object = (gameobject_t*)lua_tointeger(L, 1);
    lua_pushinteger(L, (lua_Integer)object->parent);
    return 1;
}


static int gameobject_set_string(lua_State *L)
{
    gameobject_t *object = (gameobject_t*)lua_tointeger(L, 1);
    const char *name = lua_tostring(L, 2);
    const char *value = lua_tostring(L, 3);
    object->strings[name] = std::string(value);
    return 0;
}


static int gameobject_get_string(lua_State *L)
{
    gameobject_t *object = (gameobject_t*)lua_tointeger(L, 1);
    const char *name = lua_tostring(L, 2);
    std::string value = object->strings[name];
    lua_pushstring(L, value.c_str());
    return 1;
}


static int gameobject_set_integer(lua_State *L)
{
    gameobject_t *object = (gameobject_t*)lua_tointeger(L, 1);
    const char *name = lua_tostring(L, 2);
    lua_Integer value = lua_tointeger(L, 3);
    object->integers[name] = value;
    return 0;
}

static int gameobject_get_integer(lua_State *L)
{
    gameobject_t *object = (gameobject_t*)lua_tointeger(L, 1);
    const char *name = lua_tostring(L, 2);
    lua_Integer value = object->integers[name];
    lua_pushinteger(L, value);
    return 1;
}


static int gameobject_set_number(lua_State *L)
{
    gameobject_t *object = (gameobject_t*)lua_tointeger(L, 1);
    const char *name = lua_tostring(L, 2);
    lua_Number value = lua_tonumber(L, 3);
    object->numbers[name] = value;
    return 0;
}

static int gameobject_get_number(lua_State *L)
{
    gameobject_t *object = (gameobject_t*)lua_tointeger(L, 1);
    const char *name = lua_tostring(L, 2);
    lua_Number value = object->numbers[name];
    lua_pushnumber(L, value);
    return 1;
}


static int gameobject_set_texture(lua_State *L)
{
    gameobject_t *object = (gameobject_t*)lua_tointeger(L, 1);
    object->textureIdx = lua_tointeger(L, 2);
    return 0;
}

static int gameobject_set_normal(lua_State *L)
{
    gameobject_t *object = (gameobject_t*)lua_tointeger(L, 1);
    object->textureIdx = lua_tointeger(L, 2);
    return 0;
}

static int gameobject_set_pbr(lua_State *L)
{
    gameobject_t *object = (gameobject_t*)lua_tointeger(L, 1);
    object->textureIdx = lua_tointeger(L, 2);
    return 0;
}


int EXPORT luaopen_gameobject(lua_State *L)
{
    lua_register(L, "gameobject_transform", gameobject_transform);
    lua_register(L, "gameobject_set_transform", gameobject_set_transform);
    lua_register(L, "gameobject_rotate", gameobject_rotate);
    lua_register(L, "gameobject_set_rotation", gameobject_set_rotation);
    lua_register(L, "gameobject_scale", gameobject_scale);
    lua_register(L, "gameobject_set_scale", gameobject_set_scale);
    lua_register(L, "gameobject_set_parent", gameobject_set_parent);
    lua_register(L, "gameobject_get_parent", gameobject_get_parent);
    lua_register(L, "gameobject_set_string", gameobject_set_string);
    lua_register(L, "gameobject_get_string", gameobject_get_string);
    lua_register(L, "gameobject_set_integer", gameobject_set_integer);
    lua_register(L, "gameobject_get_integer", gameobject_get_integer);
    lua_register(L, "gameobject_set_number", gameobject_set_number);
    lua_register(L, "gameobject_get_number", gameobject_get_number);
    lua_register(L, "gameobject_set_texture", gameobject_set_texture);
    lua_register(L, "gameobject_set_normal", gameobject_set_normal);
    lua_register(L, "gameobject_set_pbr", gameobject_set_pbr);
    return 0;
}

}

