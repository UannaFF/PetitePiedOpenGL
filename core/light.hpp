#ifndef LIGHT_H
#define LIGHT_H

#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include "shader.hpp"
#include "models.hpp"

//using namespace glm;

//class Shader;

class Light {
    private:
    Shader *_shader;
    Mesh *_mesh;
	glm::vec3 _pos;
	glm::vec3 _color;
    int type;
    public:
        Light();

        void setColor(glm::vec3 color) {_color = color;}
        void setPos(glm::vec3 pos) {_pos = pos;}

        glm::vec3 getColor() {return _color;}
        glm::vec3 getPos() {return _pos;}
        
        void bindAffectedObject(Shader* s, glm::vec3 ambientColor,glm::vec3 diffuseColor, glm::vec3 specular){
        	s->setVec3("light.position", _pos);
        	s->setVec3("light.ambient", ambientColor);
            s->setVec3("light.diffuse", diffuseColor);
            s->setVec3("light.specular", specular.x, specular.y, specular.z);
        }
        void draw(glm::mat4 proj, glm::mat4 view);
        Shader* shader() {return _shader;}
};

#endif
