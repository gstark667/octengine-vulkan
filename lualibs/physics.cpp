extern "C"
{
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#include "gameobject.h"
#include "pipeline.h"
#include "physics.h"


extern "C"
{

static int physics_init_box(lua_State *L)
{
    pipeline_t *scene = (pipeline_t*)lua_tointeger(L, 1);
    gameobject_t *object = (gameobject_t*)lua_tointeger(L, 2);
    float mass = lua_tonumber(L, 3);
    float x = lua_tonumber(L, 4);
    float y = lua_tonumber(L, 5);
    float z = lua_tonumber(L, 6);
    object->physics = (physics_object_t*)malloc(sizeof(physics_object_t));
    physics_object_init_box(object->physics, mass, x, y, z);
    physics_object_set_position(object->physics, object->pos.x, object->pos.y, object->pos.z);
    physics_world_add(&scene->world, object->physics);
    return 0;
}


static int physics_set_mass(lua_State *L)
{
    gameobject_t *object = (gameobject_t*)lua_tointeger(L, 1);
    float mass = lua_tonumber(L, 2);
    physics_object_set_mass(object->physics, mass);
    return 0;
}


int luaopen_physics(lua_State *L)
{
    lua_register(L, "physics_init_box", physics_init_box);
    lua_register(L, "physics_set_mass", physics_set_mass);
    return 0;
}

}

