#ifndef H_GAMEOBJECT
#define H_GAMEOBJECT

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>


struct gameobject_t
{
    glm::vec3 pos = glm::vec3(0.0f);
    glm::vec3 rot = glm::vec3(0.0f);
    float scale = 1.0f;
    uint32_t textureIdx = 0;
};

#endif
