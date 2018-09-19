#include <GLFW/glfw3.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "camera.h"

#include <iostream>
#include <cmath>


void camera_update(camera_t *camera)
{
    if (!camera->object)
        return;

    camera->view = glm::rotate(glm::mat4(1.0f), -camera->object->globalRot.x, glm::vec3(1.0f, 0.0f, 0.0f));
    camera->view *= glm::rotate(glm::mat4(1.0f), camera->object->globalRot.y, glm::vec3(0.0f, 1.0f, 0.0f));
    camera->view *= glm::rotate(glm::mat4(1.0f), camera->object->globalRot.z, glm::vec3(0.0f, 0.0f, 1.0f));
    glm::vec3 trans = glm::vec3(camera->object->globalPos.x * 0.5f, camera->object->globalPos.y * 0.5f, camera->object->globalPos.z * 0.5f);
    camera->view *= glm::inverse(glm::translate(glm::mat4(1.0f), trans));
}

void camera_resize(camera_t *camera, int width, int height, float fov)
{
    camera->proj = glm::perspective(glm::radians(fov), (float)width / (float)height, 0.1f, 1000.0f);
    camera->proj *= glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, -1.0f, 1.0f));
}

