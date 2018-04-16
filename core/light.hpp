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
    glm::vec3 _diffuse;
    glm::vec3 _ambient;
    glm::vec3 _specular;
    
    public:
        Light();

        void setColor(glm::vec3 color) {
            _color = color;
            _diffuse = _color  * glm::vec3(0.5f);
            _ambient = _diffuse * glm::vec3(0.2f);
            _specular = glm::vec3(0.1);
        }
        void setPos(glm::vec3 pos) {_pos = pos;}

        glm::vec3 getColor() {
            
            return _color;
        }
        glm::vec3 getPos() {return _pos;}
        
        void changeType() {
            if(type < 2) type++;
            else type = 0;
            printf("Change type\n");
        }
        
        void bind(Shader* s){
            s->use();
        	/*s->setVec3("light.position", _pos);
        	s->setVec3("light.ambient", _ambient);
            s->setVec3("light.diffuse", _diffuse);
            s->setVec3("light.specular", specular.x, specular.y, specular.z);*/
            glUniform3fv(s->getUniformLocation("light.position"), 1, &(_pos[0]));
            glUniform3fv(s->getUniformLocation("light.diffuse"), 1, &(_diffuse[0]));
            glUniform3fv(s->getUniformLocation("light.specular"), 1, &(_specular[0]));
            glUniform3fv(s->getUniformLocation("light.ambient"), 1, &(_ambient[0]));
            glUniform1i(s->getUniformLocation("type"), type);
            //printf("Binding light\n");
            s->deuse();
        }
        void draw(glm::mat4 proj, glm::mat4 view);
        Shader* shader() {return _shader;}
};

#endif
