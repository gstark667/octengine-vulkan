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

#include "model.h"
 
struct script_t
{
    lua_State *lua;
    std::string name;
};

void script_error(script_t *script, std::string message);
void script_create(script_t *script, std::string path);
void script_setup(script_t *script);
void script_update(script_t *script, gameobject_t *model, float delta);
void script_destroy(script_t *script);

#endif
