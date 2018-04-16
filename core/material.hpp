#ifndef MATERIAL_H
#define MATERIAL_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <assimp/material.h>
#include <assimp/texture.h>

#include <vector>
#include "texture.hpp"

class Shader;

class Material {
    public:
    
        Material(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float shininess);
        
        inline void setTextures(std::vector<Texture*>& t) { _textures = t; }
        inline std::vector<Texture*> getTextures() { return _textures; }

        void apply(Shader* usedShader, bool safemode = false);
        
        inline void setAmbient(glm::vec3 a) { _ambient = a;};
        inline void setDiffuse(glm::vec3 d) { _diffuse = d;};
        inline void setSpecular(glm::vec3 s) { _specular = s;};
        inline void setShininess(float sh) { _shininess = sh;};
        static std::vector<Texture*> loadMaterialTextures(const aiMaterial *mat, aiTextureType type, Texture::Type text_type, std::string parent_dir);
    private:
        std::vector<Texture*> _textures;
    
        glm::vec3 _ambient;
        glm::vec3 _diffuse;
        glm::vec3 _specular;    
        float _shininess;
};

#endif
