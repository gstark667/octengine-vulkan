#ifndef H_GAMEOBJECT
#define H_GAMEOBJECT

extern "C"
{
#include <lua.h>
}

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <set>
#include <map>

#include "script.h"
#include "physics.h"


struct gameobject_t
{
    glm::vec3 pos = glm::vec3(0.0f);
    glm::vec3 rot = glm::vec3(0.0f);
    float scale = 1.0f;
    uint32_t textureIdx = 0;

    glm::vec3 globalPos = glm::vec3(0.0f);
    glm::vec3 globalRot = glm::vec3(0.0f);

    gameobject_t *parent = NULL;
    physics_object_t *physics = NULL;

    std::set<script_t*> scripts;

    std::map<std::string, std::string> strings;
    std::map<std::string, lua_Integer> integers;
    std::map<std::string, lua_Number> numbers;
};

void gameobject_update_global(gameobject_t *gameobject);
void gameobject_update(gameobject_t *gameobject, void *scene, float delta);

#endif
