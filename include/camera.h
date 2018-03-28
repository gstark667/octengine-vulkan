#ifndef H_CAMERA
#define H_CAMERA


struct camera_t
{
    float fov;
    float width, height;

    double mouseX, mouseY;
    glm::vec3 pos;
    glm::vec2 rot;

    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
};

void camera_update(camera_t *camera, float delta, GLFWwindow *window);

#endif
