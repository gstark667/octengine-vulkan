#include <GLFW/glfw3.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "camera.h"

#include <iostream>


void camera_update(camera_t *camera)
{
    if (!camera->object)
        return;

    camera->view = glm::rotate(glm::mat4(1.0f), camera->object->rot.x, glm::vec3(1.0f, 0.0f, 0.0f));
    camera->view *= glm::rotate(glm::mat4(1.0f), camera->object->rot.y, glm::vec3(0.0f, 1.0f, 0.0f));
    camera->view *= glm::rotate(glm::mat4(1.0f), camera->object->rot.z, glm::vec3(0.0f, 0.0f, 1.0f));
    camera->view *= glm::translate(glm::mat4(1.0f), camera->object->pos);
}

void camera_resize(camera_t *camera, int width, int height, float fov)
{
    camera->proj = glm::perspective(glm::radians(fov), (float)width / (float)height, 0.1f, 1000.0f);
}

