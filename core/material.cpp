#include "material.hpp"
#include "shader.hpp"
#include "common.hpp"

Material::Material(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float shininess):
    _ambient(ambient), _diffuse(diffuse), _specular(specular), _shininess(shininess), _shading_mode(None), _textures()
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

bool Material::setShadingMode(aiShadingMode m){
    switch (m){
    case aiShadingMode_Phong:
        _shading_mode = Phong;
        return true;
    case aiShadingMode_Fresnel:
        _shading_mode = Fresnel;
        return true;
    case aiShadingMode_NoShading:
        _shading_mode = None;
        return true;
        
  //~ aiShadingMode_Flat = 0x1, aiShadingMode_Gouraud = 0x2, aiShadingMode_Phong = 0x3, aiShadingMode_Blinn = 0x4,
  //~ aiShadingMode_Toon = 0x5, aiShadingMode_OrenNayar = 0x6, aiShadingMode_Minnaert = 0x7, aiShadingMode_CookTorrance = 0x8,
  //~ aiShadingMode_NoShading = 0x9, aiShadingMode_Fresnel = 0xa
    }
    return false;
}


void Material::apply(Shader* shader){
    // TODO: Phong, and Fresnel
    
    tex_bind_t binder = {1, 1, 1, 1};
    glUniform3fv(shader->getUniformLocation("material.ambient"), 1, &(_ambient[0]));
    glUniform3fv(shader->getUniformLocation("material.diffuse"), 1, &(_diffuse[0]));
    glUniform3fv(shader->getUniformLocation("material.specular"), 1, &(_specular[0]));
    glUniform1f(shader->getUniformLocation("material.shininess"), _shininess);
    
    for(unsigned int i = 0; i < _textures.size(); i++){
        Texture* t = _textures[i];
        //~ glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
        // retrieve texture number (the N in diffuse_textureN)
        
        std::string fragment_name = "texture_";
        
        switch (t->type()){
        case Texture::Diffuse:
            fragment_name += "diffuse" + std::to_string(binder.diffuse++);
            break;
        case Texture::Specular:
            fragment_name += "specular" + std::to_string(binder.specular++);
            break;
        case Texture::Normal:
            fragment_name += "normal" + std::to_string(binder.normal++);
            break;
        case Texture::Height:
            fragment_name += "height" + std::to_string(binder.height++);
            break;
        case Texture::Cube:
            fragment_name += "cube";
            break;
        }

                 
    
        DEBUG(Debug::info, "Applying material '%s'(%d) and idtex %d\n", fragment_name.c_str(), shader->getUniformLocation(fragment_name.c_str()), t->texId());                                // now set the sampler to the correct texture unit
        //~ glUniform1i(shader->getUniformLocation(fragment_name.c_str()), i);
        // and finally bind the texture
        //~ glBindTexture(GL_TEXTURE_2D, t->id());
        t->apply(shader->getUniformLocation(fragment_name.c_str()));
    }
}
