#ifndef H_LIGHT
#define H_LIGHT

#include "camera.h"


struct light_t
{
    camera_t camera;
    glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
    float brightness = 1.0f;
    bool point = false;
    int shadowIdx = -1;
};

#endif
