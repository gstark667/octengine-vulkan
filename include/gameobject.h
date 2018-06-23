#ifndef H_GAMEOBJECT
#define H_GAMEOBJECT

extern "C"
{
#include <lua.h>
}

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <set>
#include <map>

#include "script.h"


struct gameobject_t
{
    glm::vec3 pos = glm::vec3(0.0f);
    glm::vec3 rot = glm::vec3(0.0f);
    float scale = 1.0f;
    uint32_t textureIdx = 0;

    std::set<script_t*> scripts;

    std::map<std::string, std::string> strings;
    std::map<std::string, lua_Integer> integers;
    std::map<std::string, lua_Number> numbers;
};

#endif
