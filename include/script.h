#ifndef H_SCRIPT
#define H_SCRIPT

extern "C"
{
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

#include <string>
#include <iostream>
#include <stdlib.h>

struct event_t;

#include "settings.h"

 
struct script_t
{
    lua_State *lua;
    std::string name;
};

void script_error(script_t *script, std::string message);
void script_create(script_t *script, std::string path);
void script_setup(script_t *script, void *scene, void *object);
void script_update(script_t *script, void *scene, void *object, float delta);
void script_on_cursor_pos(script_t *script, void *scene, void *object, double x, double y);
void script_on_button_down(script_t *script, void *scene, void *object, std::string buttonCode);
void script_on_button_up(script_t *script, void *scene, void *object, std::string buttonCode);
void script_on_event(script_t *script, void *scene, void *object, event_t event);
void script_on_collision_enter(script_t *script, void *scene, void *object, void *other);
void script_on_collision_exit(script_t *script, void *scene, void *object, void *other);
void script_destroy(script_t *script);

#endif
