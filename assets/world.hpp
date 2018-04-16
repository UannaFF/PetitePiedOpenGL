
#ifndef WORLD_H
#define WORLD_H
#include <glm/glm.hpp>
#include "../core/light.hpp"

class Scene;

class DinoWorld {
    public:
        static Scene* buildScene();
        Light* _light;
};

glm::mat4 translation(float x, float y, float z);

#endif
