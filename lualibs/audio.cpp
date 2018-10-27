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

#include "audio.h"
#include "scene.h"


extern "C"
{

static int audio_source_create(lua_State *L)
{
    scene_t *scene = (scene_t*)lua_tointeger(L, 1);
    gameobject_t *object = (gameobject_t*)lua_tointeger(L, 2);
    const char *path = lua_tostring(L, 3);
    ALuint source = audio_source_create(&scene->audio, object, path);
    lua_pushinteger(L, (lua_Integer)source);
    return 1;
}

static int audio_source_play(lua_State *L)
{
    ALuint source = lua_tointeger(L, 1);
    alSourcePlay(source);
    return 0;
}

static int audio_source_set_pitch(lua_State *L)
{
    ALuint source = lua_tointeger(L, 1);
    float pitch = lua_tonumber(L, 2);
    alSourcef(source, AL_PITCH, pitch);
    return 0;
}

static int audio_source_set_gain(lua_State *L)
{
    ALuint source = lua_tointeger(L, 1);
    float gain = lua_tonumber(L, 2);
    alSourcef(source, AL_GAIN, gain);
    return 0;
}


int EXPORT luaopen_audio(lua_State *L)
{
    lua_register(L, "audio_source_create", audio_source_create);
    lua_register(L, "audio_source_play", audio_source_play);
    lua_register(L, "audio_source_set_pitch", audio_source_set_pitch);
    lua_register(L, "audio_source_set_gain", audio_source_set_gain);
    return 0;
}

}

