#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>

#include "luaheaders.h"
#include "export.h"
#include "scene.h"
#include "ui.h"


extern "C"
{

static int libui_element_create(lua_State *L)
{
    scene_t *scene = (scene_t*)lua_tointeger(L, 1);
    scene->isDirty = true;
    ui_element_t *parent = (ui_element_t*)lua_tointeger(L, 2);

    if (!parent)
        parent = scene->ui.root;

    ui_element_t *element = ui_element_create(&scene->ui, parent);
    lua_pushinteger(L, (lua_Integer)element);
    return 1;
}

static int libui_element_size(lua_State *L)
{
    ui_element_t *element = (ui_element_t*)lua_tointeger(L, 1);
    float width = lua_tonumber(L, 2);
    float height = lua_tonumber(L, 3);

    ui_element_size(element, width, height);
    return 0;
}

static int libui_element_allign(lua_State *L)
{
    ui_element_t *element = (ui_element_t*)lua_tointeger(L, 1);
    const char *side = lua_tostring(L, 2);
    float offset = lua_tonumber(L, 3);

    if (strcmp(side, "left") == 0)
        element->x = -1 + element->width + offset;
    else if (strcmp(side, "right") == 0)
        element->x = 1 - element->width - offset;
    else if (strcmp(side, "top") == 0)
        element->y = -1 + element->height + offset;
    else if (strcmp(side, "bottom") == 0)
        element->y = 1 - element->height - offset;

    return 0;
}

static int libui_element_set_text(lua_State *L)
{
    ui_element_t *element = (ui_element_t*)lua_tointeger(L, 1);
    const char *text = lua_tostring(L, 2);
    element->text = std::string(text);

    return 0;
}

int EXPORT luaopen_ui(lua_State *L)
{
    lua_register(L, "ui_element_create", libui_element_create);
    lua_register(L, "ui_element_size", libui_element_size);
    lua_register(L, "ui_element_allign", libui_element_allign);
    lua_register(L, "ui_element_set_text", libui_element_set_text);
    return 0;
}

}

