#include "material.hpp"
#include "shader.hpp"
#include "common.hpp"

Material::Material(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float shininess):
    _ambient(ambient), _diffuse(diffuse), _specular(specular), _shininess(shininess), _textures()
{
}

std::vector<Texture*> Material::loadMaterialTextures(const aiMaterial *mat, aiTextureType type, Texture::Type text_type, std::string parent_dir){
    std::vector<Texture*> textures;
    aiString str;
    
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++){
        mat->GetTexture(type, i, &str);
    
        Texture* texture;
        
        auto it = Texture::LOADED.find(str.C_Str());
        
        if(it == Texture::LOADED.end()){   // if texture hasn't been loaded already, load it
            texture = Texture::fromFile(str.C_Str(), parent_dir);
            texture->type(text_type);
            Texture::LOADED.insert(std::pair<std::string, Texture*>(str.C_Str(), texture));  // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
        }
        else
            texture = it->second;
    
        textures.push_back(texture);
    }
    
    return textures;
}


void Material::apply(Shader* shader, bool safemode){

    try {
        shader->setVec3("material.ambient", _ambient);
        shader->setVec3("material.diffuse", _diffuse);
        shader->setVec3("material.specular", _specular);
        shader->setFloat("material.shininess", _shininess);

        //init
        shader->setBool("has_diffuse", false);
        shader->setBool("has_specular", false);
        shader->setBool("has_normal", false);
        shader->setBool("has_height", false);
    } catch (ShaderException* s){
        if (!safemode)
            throw s;
    }
    
    for(unsigned int i = 0; i < _textures.size(); i++){
        Texture* t = _textures[i];
        
        std::string fragment_name;
        
        switch (t->type()){
        case Texture::Diffuse:
            fragment_name = "diffuse";
            break;
        case Texture::Specular:
            fragment_name = "specular";
            break;
        case Texture::Normal:
            fragment_name = "normal";
            break;
        case Texture::Height:
            fragment_name = "height";
            break;
        case Texture::Cube:
            fragment_name = "cube";
            break;
        }

        t->apply(shader->getUniformLocation("texture_" + fragment_name));
        if(t->type() != Texture::Cube)
            shader->setBool("has_"+fragment_name, true);
            
        Debug::CheckOpenGLError("Material applied");
    }
}

