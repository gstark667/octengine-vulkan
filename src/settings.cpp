#include "settings.h"
#include "script.h"


void settings_create(settings_t *settings)
{
    script_t script;
    script_create(&script, "player_settings.lua");

    lua_getglobal(script.lua, "keybinds");
    lua_pushnil(script.lua);

    while(lua_next(script.lua, -2) != 0)
    {
        if(lua_istable(script.lua, -1))
        {
            std::string key = std::string(lua_tostring(script.lua, -2));
            std::cout << key << std::endl;
            lua_pushnil(script.lua);
            while (lua_next(script.lua, -2) != 0)
            {
                std::string name(lua_tostring(script.lua, -2));
                lua_Number value = lua_tonumber(script.lua, -1);
                settings->keybinds[key].push_back({name, value});
                settings->events[name] = 0;
                lua_pop(script.lua, 1);
            }
        }
        lua_pop(script.lua, 1);
    }

    script_destroy(&script);
}

void settings_on_button(settings_t *settings, std::string code, bool down)
{
    if (settings->keybinds.find(code) == settings->keybinds.end())
        return;
    for (auto it = settings->keybinds[code].begin(); it != settings->keybinds[code].end(); ++it)
    {
        settings->triggeredEvents.insert(it->name);
        settings->events[it->name] += down ? it->value : -it->value;
    }
}

void settings_on_axis(settings_t *settings, std::string code, float value)
{
    if (settings->keybinds.find(code) == settings->keybinds.end())
        return;
    for (auto it = settings->keybinds[code].begin(); it != settings->keybinds[code].end(); ++it)
    {
        settings->triggeredEvents.insert(it->name);
        settings->events[it->name] = it->value * value;
    }
}

