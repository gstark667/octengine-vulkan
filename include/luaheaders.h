#ifndef H_LUAHEADERS
#define H_LUAHEADERS


extern "C"
{
#ifdef __APPLE__
#include <lua/lua.h>
#include <lua/luaxlib.h>
#include <lua/lualib.h>
#else
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#endif
}

#endif
