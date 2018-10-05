#ifndef H_SETTINGS
#define H_SETTINGS

#include <map>
#include <set>
#include <vector>

#include "script.h"


struct event_t
{
    std::string name;
    lua_Number value;
};

struct settings_t
{
    std::map<std::string, std::vector<event_t>> keybinds;
    std::map<std::string, lua_Number> events;
    std::set<std::string> triggeredEvents;
};

void settings_create(settings_t *settings);
void settings_on_button(settings_t *settings, std::string code, bool down);
void settings_on_axis(settings_t *settings, std::string code, float value);

#endif
