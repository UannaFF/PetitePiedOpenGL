
#ifndef WORLD_H
#define WORLD_H
#include <glm/glm.hpp>

class Scene;

class DinoWorld {
    public:
        static Scene* buildScene();
};

glm::mat4 translation(float x, float y, float z);

#endif
