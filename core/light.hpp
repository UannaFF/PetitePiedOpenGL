#ifndef LIGHT_H
#define LIGHT_H

#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include "shader.hpp"
#include "models.hpp"

class Light {    
    public:
        enum Type { Phong, PhongWithNormal};
        Light();

        void setColor(glm::vec3 color) {
            _color = color;
            _diffuse = _color  * glm::vec3(0.5f);
            _ambient = _diffuse * glm::vec3(0.2f);
            _specular = glm::vec3(0.1);
        }
        
        inline void setPos(glm::vec3 pos) { _pos = pos; }

        inline glm::vec3 getColor() { return _color; }
        inline glm::vec3 getPos() { return _pos; }
        
        inline void type(Type t) { _type = t; }
        inline Type type() const { return _type; }
        
        void bind(Shader* s){
            s->use();
            s->setVec3("light.position", _pos);
            s->setVec3("light.diffuse", _diffuse);
            s->setVec3("light.specular", _specular);
            s->setVec3("light.ambient", _ambient);
            s->setInt("light.type", (int)_type);
            s->deuse();
        }
        
        void draw(glm::mat4 proj, glm::mat4 view);
        Shader* shader() {return _shader;}
    
    private:
        Shader *_shader;
        Mesh *_mesh;
        
        Type _type;
        
        glm::vec3 _pos;
        glm::vec3 _color;
        
        glm::vec3 _diffuse;
        glm::vec3 _ambient;
        glm::vec3 _specular;
};

#endif
