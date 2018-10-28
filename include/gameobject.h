#ifndef H_GAMEOBJECT
#define H_GAMEOBJECT

extern "C"
{
/* OSX brew puts lua headers under /usr/local/include/lua */
#include <lua/lua.h>
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
#include "settings.h"

struct gameobject_t;

#include "model.h"

#include "export.h"


struct gameobject_t
{
    glm::vec3 pos = glm::vec3(0.0f);
    glm::vec3 rot = glm::vec3(0.0f);
    glm::vec3 scale = glm::vec3(1.0f);
    glm::ivec3 textureIdx = glm::ivec3(0, 1, 2);

    glm::vec3 globalPos = glm::vec3(0.0f);
    glm::vec3 globalRot = glm::vec3(0.0f);

    model_t *model = NULL;
    gameobject_t *parent = NULL;
    physics_object_t *physics = NULL;

    std::set<script_t*> scripts;

    std::map<std::string, std::string> strings;
    std::map<std::string, lua_Integer> integers;
    std::map<std::string, lua_Number> numbers;
};

extern "C" {
void EXPORT gameobject_update_global(gameobject_t *gameobject);
void EXPORT gameobject_update(gameobject_t *gameobject, void *scene, float delta);
void EXPORT gameobject_on_cursor_pos(gameobject_t *gameobject, void *scene, double x, double y);
void EXPORT gameobject_on_button_down(gameobject_t *gameobject, void *scene, std::string buttonCode);
void EXPORT gameobject_on_button_up(gameobject_t *gameobject, void *scene, std::string buttonCode);
void EXPORT gameobject_on_event(gameobject_t *gameobject, void *scene, event_t event);
void EXPORT gameobject_on_collision_enter(gameobject_t *gameobject, gameobject_t *other, void *scene);
void EXPORT gameobject_on_collision_exit(gameobject_t *gameobject, gameobject_t *other, void *scene);
glm::vec3 EXPORT gameobject_rotate_vector(gameobject_t *gameobject, glm::vec3 vector);
glm::vec3 EXPORT gameobject_unrotate_vector(gameobject_t *gameobject, glm::vec3 vector);
}

#endif
