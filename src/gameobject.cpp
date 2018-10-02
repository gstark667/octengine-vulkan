#include "gameobject.h"


void gameobject_update_global(gameobject_t *gameobject)
{
    if (gameobject->physics)
    {
        gameobject->globalPos = physics_object_get_position(gameobject->physics);
        gameobject->globalRot = physics_object_get_rotation(gameobject->physics);
    }
    else if (gameobject->parent)
    {
        gameobject_update_global(gameobject->parent);

        glm::mat4 matrix = glm::rotate(glm::mat4(1.0f), gameobject->parent->globalRot.z, glm::vec3(0.0f, 0.0f, 1.0f));
        matrix *= glm::rotate(glm::mat4(1.0f), gameobject->parent->globalRot.y, glm::vec3(0.0f, 1.0f, 0.0f));
        matrix *= glm::rotate(glm::mat4(1.0f), gameobject->parent->globalRot.x, glm::vec3(1.0f, 0.0f, 0.0f));

        glm::mat4 trans = glm::translate(glm::mat4(1.0f), gameobject->parent->globalPos);

        glm::vec4 newPos =  trans * (matrix * glm::vec4(gameobject->pos, 1.0f));

        gameobject->globalPos = glm::vec3(newPos.x, newPos.y, newPos.z);
        gameobject->globalRot = gameobject->rot + gameobject->parent->globalRot;
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

void gameobject_on_cursor_pos(gameobject_t *gameobject, void *scene, double x, double y)
{
    for (std::set<script_t*>::iterator it = gameobject->scripts.begin(); it != gameobject->scripts.end(); ++it)
    {
        script_on_cursor_pos(*it, scene, gameobject, x, y);
    }
}

void gameobject_on_button_down(gameobject_t *gameobject, void *scene, std::string buttonCode)
{
    for (std::set<script_t*>::iterator it = gameobject->scripts.begin(); it != gameobject->scripts.end(); ++it)
    {
        script_on_button_down(*it, scene, gameobject, buttonCode);
    }
}

void gameobject_on_button_up(gameobject_t *gameobject, void *scene, std::string buttonCode)
{
    for (std::set<script_t*>::iterator it = gameobject->scripts.begin(); it != gameobject->scripts.end(); ++it)
    {
        script_on_button_up(*it, scene, gameobject, buttonCode);
    }
}

void gameobject_on_collision_enter(gameobject_t *gameobject, gameobject_t *other, void *scene)
{
    for (std::set<script_t*>::iterator it = gameobject->scripts.begin(); it != gameobject->scripts.end(); ++it)
    {
        script_on_collision_enter(*it, scene, gameobject, other);
    }
}

void gameobject_on_collision_exit(gameobject_t *gameobject, gameobject_t *other, void *scene)
{
    for (std::set<script_t*>::iterator it = gameobject->scripts.begin(); it != gameobject->scripts.end(); ++it)
    {
        script_on_collision_exit(*it, scene, gameobject, other);
    }
}

