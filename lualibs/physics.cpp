#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#include "luaheaders.h"
#include "export.h"
#include "gameobject.h"
#include "scene.h"
#include "physics.h"


extern "C"
{

static int physics_init_box(lua_State *L)
{
    scene_t *scene = (scene_t*)lua_tointeger(L, 1);
    gameobject_t *object = (gameobject_t*)lua_tointeger(L, 2);
    float mass = lua_tonumber(L, 3);
    float x = lua_tonumber(L, 4);
    float y = lua_tonumber(L, 5);
    float z = lua_tonumber(L, 6);
    object->physics = (physics_object_t*)malloc(sizeof(physics_object_t));
    physics_object_init_box(object->physics, object, mass, x, y, z);
    physics_object_set_position(object->physics, object->pos.x, object->pos.y, object->pos.z);
    physics_world_add(&scene->world, object->physics);
    return 0;
}

static int physics_init_sphere(lua_State *L)
{
    scene_t *scene = (scene_t*)lua_tointeger(L, 1);
    gameobject_t *object = (gameobject_t*)lua_tointeger(L, 2);
    float mass = lua_tonumber(L, 3);
    float radius = lua_tonumber(L, 4);
    object->physics = (physics_object_t*)malloc(sizeof(physics_object_t));
    physics_object_init_sphere(object->physics, object, mass, radius);
    physics_object_set_position(object->physics, object->pos.x, object->pos.y, object->pos.z);
    physics_world_add(&scene->world, object->physics);
    return 0;
}

static int physics_init_capsule(lua_State *L)
{
    scene_t *scene = (scene_t*)lua_tointeger(L, 1);
    gameobject_t *object = (gameobject_t*)lua_tointeger(L, 2);
    float mass = lua_tonumber(L, 3);
    float radius = lua_tonumber(L, 4);
    float height = lua_tonumber(L, 5);
    object->physics = (physics_object_t*)malloc(sizeof(physics_object_t));
    physics_object_init_capsule(object->physics, object, mass, radius, height);
    physics_object_set_position(object->physics, object->pos.x, object->pos.y, object->pos.z);
    physics_world_add(&scene->world, object->physics);
    return 0;
}

static int physics_init_convex_hull(lua_State *L)
{
    scene_t *scene = (scene_t*)lua_tointeger(L, 1);
    gameobject_t *object = (gameobject_t*)lua_tointeger(L, 2);
    float mass = lua_tonumber(L, 3);
    object->physics = (physics_object_t*)malloc(sizeof(physics_object_t));
    physics_object_init_convex_hull(object->physics, object, mass);
    physics_object_set_position(object->physics, object->pos.x, object->pos.y, object->pos.z);
    physics_world_add(&scene->world, object->physics);
    return 0;
}

static int physics_init_mesh(lua_State *L)
{
    scene_t *scene = (scene_t*)lua_tointeger(L, 1);
    gameobject_t *object = (gameobject_t*)lua_tointeger(L, 2);
    float mass = lua_tonumber(L, 3);
    object->physics = (physics_object_t*)malloc(sizeof(physics_object_t));
    physics_object_init_mesh(object->physics, object, mass);
    physics_object_set_position(object->physics, object->pos.x, object->pos.y, object->pos.z);
    physics_world_add(&scene->world, object->physics);
    return 0;
}

static int physics_set_position(lua_State *L)
{
    gameobject_t *object = (gameobject_t*)lua_tointeger(L, 1);
    float x = lua_tonumber(L, 2);
    float y = lua_tonumber(L, 3);
    float z = lua_tonumber(L, 4);
    physics_object_set_position(object->physics, x, y, z);
    return 0;
}

static int physics_set_rotation(lua_State *L)
{
    gameobject_t *object = (gameobject_t*)lua_tointeger(L, 1);
    float x = lua_tonumber(L, 2);
    float y = lua_tonumber(L, 3);
    float z = lua_tonumber(L, 4);
    physics_object_set_rotation(object->physics, x, y, z);
    return 0;
}

static int physics_get_rotation(lua_State *L)
{
    gameobject_t *object = (gameobject_t*)lua_tointeger(L, 1);
    glm::vec3 rot = physics_object_get_rotation(object->physics);
    lua_pushnumber(L, rot.x);
    lua_pushnumber(L, rot.y);
    lua_pushnumber(L, rot.z);
    return 3;
}

static int physics_set_mass(lua_State *L)
{
    gameobject_t *object = (gameobject_t*)lua_tointeger(L, 1);
    float mass = lua_tonumber(L, 2);
    physics_object_set_mass(object->physics, mass);
    return 0;
}

static int physics_get_velocity(lua_State *L)
{
    gameobject_t *object = (gameobject_t*)lua_tointeger(L, 1);
    glm::vec3 velocity = physics_object_get_velocity(object->physics);
    lua_pushnumber(L, velocity.x);
    lua_pushnumber(L, velocity.y);
    lua_pushnumber(L, velocity.z);
    return 3;
}

static int physics_set_velocity(lua_State *L)
{
    gameobject_t *object = (gameobject_t*)lua_tointeger(L, 1);
    float x = lua_tonumber(L, 2);
    float y = lua_tonumber(L, 3);
    float z = lua_tonumber(L, 4);
    physics_object_set_velocity(object->physics, x, y, z);
    return 0;
}

static int physics_get_angular_velocity(lua_State *L)
{
    gameobject_t *object = (gameobject_t*)lua_tointeger(L, 1);
    glm::vec3 velocity = physics_object_get_angular_velocity(object->physics);
    lua_pushnumber(L, velocity.x);
    lua_pushnumber(L, velocity.y);
    lua_pushnumber(L, velocity.z);
    return 3;
}

static int physics_set_angular_velocity(lua_State *L)
{
    gameobject_t *object = (gameobject_t*)lua_tointeger(L, 1);
    float x = lua_tonumber(L, 2);
    float y = lua_tonumber(L, 3);
    float z = lua_tonumber(L, 4);
    physics_object_set_angular_velocity(object->physics, x, y, z);
    return 0;
}

static int physics_apply_force(lua_State *L)
{
    gameobject_t *object = (gameobject_t*)lua_tointeger(L, 1);
    float fx = lua_tonumber(L, 2);
    float fy = lua_tonumber(L, 3);
    float fz = lua_tonumber(L, 4);
    float px = lua_tonumber(L, 5);
    float py = lua_tonumber(L, 6);
    float pz = lua_tonumber(L, 7);
    physics_object_apply_force(object->physics, fx, fy, fz, px, py, pz);
    return 0;
}

static int physics_set_angular_factor(lua_State *L)
{
    gameobject_t *object = (gameobject_t*)lua_tointeger(L, 1);
    float factor = lua_tonumber(L, 2);
    physics_object_set_angular_factor(object->physics, factor);
    return 0;
}

static int physics_ray_test(lua_State *L)
{
    scene_t *scene = (scene_t*)lua_tointeger(L, 1);
    float fx = lua_tonumber(L, 2);
    float fy = lua_tonumber(L, 3);
    float fz = lua_tonumber(L, 4);
    float tx = lua_tonumber(L, 5);
    float ty = lua_tonumber(L, 6);
    float tz = lua_tonumber(L, 7);
    ray_hit_t hit = physics_world_ray_test(&scene->world, glm::vec3(fx, fy, fz), glm::vec3(tx, ty, tz));

    lua_pushboolean(L, hit.hit);
    lua_pushnumber(L, hit.dist);
    lua_pushnumber(L, hit.point.x);
    lua_pushnumber(L, hit.point.y);
    lua_pushnumber(L, hit.point.z);
    lua_pushnumber(L, hit.normal.x);
    lua_pushnumber(L, hit.normal.y);
    lua_pushnumber(L, hit.normal.z);
    return 8;
}


int EXPORT luaopen_physics(lua_State *L)
{
    lua_register(L, "physics_init_box", physics_init_box);
    lua_register(L, "physics_init_sphere", physics_init_sphere);
    lua_register(L, "physics_init_capsule", physics_init_capsule);
    lua_register(L, "physics_init_convex_hull", physics_init_convex_hull);
    lua_register(L, "physics_init_mesh", physics_init_mesh);
    lua_register(L, "physics_set_position", physics_set_position);
    lua_register(L, "physics_set_rotation", physics_set_rotation);
    lua_register(L, "physics_get_rotation", physics_get_rotation);
    lua_register(L, "physics_set_mass", physics_set_mass);
    lua_register(L, "physics_get_velocity", physics_get_velocity);
    lua_register(L, "physics_set_velocity", physics_set_velocity);
    lua_register(L, "physics_get_angular_velocity", physics_get_angular_velocity);
    lua_register(L, "physics_set_angular_velocity", physics_set_angular_velocity);
    lua_register(L, "physics_apply_force", physics_apply_force);
    lua_register(L, "physics_set_angular_factor", physics_set_angular_factor);
    lua_register(L, "physics_ray_test", physics_ray_test);
    return 0;
}

}

