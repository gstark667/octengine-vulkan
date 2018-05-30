#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "camera.h"

#include <iostream>


void camera_update(camera_t *camera, float delta, GLFWwindow *window)
{
    double newMouseX, newMouseY;
    glfwGetCursorPos(window, &newMouseX, &newMouseY);

    camera->rot.x += (float)(newMouseX - camera->mouseX) * 0.005f;
    camera->rot.y -= (float)(newMouseY - camera->mouseY) * 0.005f;
    if (camera->rot.y > 1.57079632679f)
        camera->rot.y = 1.57079632679;
    if (camera->rot.y < -1.57079632679f)
        camera->rot.y = -1.57079632679;

    camera->mouseX = newMouseX;
    camera->mouseY = newMouseY;

    float speed = 5.0f;
    glm::vec3 velocityVec(0.0f);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        velocityVec.x -= delta * speed;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        velocityVec.x += delta * speed;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        velocityVec.z += delta * speed;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        velocityVec.z -= delta * speed;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        velocityVec.y += delta * speed;
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        velocityVec.y -= delta * speed;
    double velocity = sqrt(velocityVec.x * velocityVec.x + velocityVec.y * velocityVec.y);
    camera->pos.x += (float)(cos(camera->rot.x) * velocityVec.x);
    camera->pos.x -= (float)(sin(camera->rot.x) * velocityVec.z);
    camera->pos.z += (float)(sin(camera->rot.x) * velocityVec.x);
    camera->pos.z += (float)(cos(camera->rot.x) * velocityVec.z);
    camera->pos.y += velocityVec.y;

    std::cout << camera->pos.x << ":" << camera->pos.y << ":" << camera->pos.z << std::endl;
 
    camera->view = glm::rotate(glm::mat4(1.0f), camera->rot.y, glm::vec3(1.0f, 0.0f, 0.0f));
    camera->view *= glm::rotate(glm::mat4(1.0f), camera->rot.x, glm::vec3(0.0f, 1.0f, 0.0f));
    camera->view *= glm::translate(glm::mat4(1.0f), glm::vec3(camera->pos.x, camera->pos.y, camera->pos.z));
    camera->proj = glm::perspective(glm::radians(camera->fov), (float)camera->width / (float)camera->height, 0.1f, 1000.0f);
}

