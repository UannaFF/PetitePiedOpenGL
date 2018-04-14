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

typedef struct tex_bind_struct {    
    unsigned int diffuse;
    unsigned int specular;
    unsigned int normal;
    unsigned int height;
} tex_bind_t;

class Material {
    public:
        enum ShadingMode {None, Phong, Fresnel};
    
        Material(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float shininess);
        
        inline void setTextures(std::vector<Texture*>& t) { _textures = t; }
        
        bool setShadingMode(aiShadingMode m);
        
        void apply(Shader* usedShader);
        
        static std::vector<Texture*> loadMaterialTextures(const aiMaterial *mat, aiTextureType type, Texture::Type text_type, std::string parent_dir);
    private:
        std::vector<Texture*> _textures;
        
        ShadingMode _shading_mode;
    
        glm::vec3 _ambient;
        glm::vec3 _diffuse;
        glm::vec3 _specular;    
        float _shininess;
};

#endif
