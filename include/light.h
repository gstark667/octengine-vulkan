#ifndef H_LIGHT
#define H_LIGHT

#include "camera.h"


struct light_t
{
    camera_t camera;
    glm::vec3 color;
};

#endif
