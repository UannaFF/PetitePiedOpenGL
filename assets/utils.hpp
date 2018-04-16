#ifndef ARM_H
#define ARM_H

#include "../core/scene.hpp"
#include "../core/models.hpp"
#include "../core/material.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include <vector>
#include <string>

class Shader;
class Material;
class ReferenceMarker: public Mesh {
    public:
        ReferenceMarker(Shader* s);
                
        void draw(glm::mat4 projection, glm::mat4 view, glm::mat4 model);
};

class TextureLoader {
    private:
        static std::vector<std::string> TEXTURED_NODE; 
        static std::map<std::string, Material*> MATERIALS;
    public:
        TextureLoader();
        void loadTextures(Node *node);
};


#endif
