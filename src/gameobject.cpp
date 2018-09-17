#include "gameobject.h"


void gameobject_update_global(gameobject_t *gameobject)
{
    if (gameobject->parent)
    {
        gameobject_update_global(gameobject->parent);

        //gameobject->globalRot = gameobject->parent->globalRot;

        glm::mat4 matrix = glm::rotate(glm::mat4(1.0f), gameobject->parent->rot.x, glm::vec3(1.0f, 0.0f, 0.0f));
        matrix *= glm::rotate(glm::mat4(1.0f), gameobject->parent->rot.y, glm::vec3(0.0f, 1.0f, 0.0f));
        matrix *= glm::rotate(glm::mat4(1.0f), gameobject->parent->rot.z, glm::vec3(0.0f, 0.0f, 1.0f));
        matrix *= glm::translate(glm::mat4(1.0f), gameobject->parent->pos);

        glm::vec4 newPos = glm::vec4(gameobject->pos, 0.0f) * matrix;

        gameobject->globalPos = glm::vec3(newPos.x, newPos.y, newPos.z);
    }
    else
    {
        gameobject->globalPos = gameobject->pos;
        gameobject->globalRot = gameobject->rot;
    }
}

void gameobject_update(gameobject_t *gameobject, void *scene, float delta)
{
    for (std::set<script_t*>::iterator it = gameobject->scripts.begin(); it != gameobject->scripts.end(); ++it)
    {
        script_update(*it, scene, gameobject, delta);
    }
}
