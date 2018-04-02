#ifndef H_SCENE
#define H_SCENE

#include "texture.h"
#include "model.h"

#include <vector>


struct scene_t
{
    std::vector<texture_t*> textures;
    std::vector<model_t*> models;
};

#endif
