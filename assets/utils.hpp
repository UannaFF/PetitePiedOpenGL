#ifndef ARM_H
#define ARM_H

#include "../core/models.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

class Shader;

class ReferenceMarker: public Mesh {
    public:
        ReferenceMarker(Shader* s);
                
        void draw(glm::mat4 projection, glm::mat4 view, glm::mat4 model);
};

#endif
