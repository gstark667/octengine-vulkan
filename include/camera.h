#ifndef H_CAMERA
#define H_CAMERA

#include "gameobject.h"


struct camera_t
{
    gameobject_t *object = NULL;
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 proj = glm::mat4(1.0f);
    float fov = 90.0f;
    int width = 1;
    int height = 1;
    float sizeX = 10.0f;
    float sizeY = 10.0f;
};

void camera_update(camera_t *camera);
void camera_resize(camera_t *camera);

#endif
