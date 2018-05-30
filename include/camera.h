#ifndef H_CAMERA
#define H_CAMERA


struct camera_t
{
    float fov = 45.0f;
    float width = 0.0f;
    float height = 0.0f;

    double mouseX = 0.0;
    double mouseY = 0.0;
    glm::vec3 pos = glm::vec3(0.0f);
    glm::vec2 rot = glm::vec3(0.0f);

    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 proj = glm::mat4(1.0f);
};

void camera_update(camera_t *camera, float delta, GLFWwindow *window);

#endif
