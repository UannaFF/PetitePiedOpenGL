#include "utils.hpp"
#include "../core/shader.hpp"
#include <iostream>
#include <vector>

ReferenceMarker::ReferenceMarker(Shader* s):
    Mesh(s, new VertexArray, std::vector<Bone*>())
{
    std::vector<float> pos_color({0, 0, 0,
                                  3, 0, 0,
                                  0, 0, 0,
                                  0, -3, 0,
                                  0, 0, 0,
                                  0, 0, 3});
                                  
    VAO()->setVertex(pos_color);  
}
                
void ReferenceMarker::draw(glm::mat4 projection, glm::mat4 view, glm::mat4 model){
    _shader->use();    

    // setup camera geometry parameters
    _shader->setMat4("projection", projection);
    _shader->setMat4("view", view);
    _shader->setMat4("model", model, GL_TRUE);
    
    // draw mesh vertex array
    VAO()->draw(GL_LINES);

    // leave with clean OpenGL state, to make it easier to detect problems
    _shader->deuse();
}

std::vector<std::string> TextureLoader::TEXTURED_NODE(
    {"island", "rock", "tree", "water", "diplodocus", "plane", "boat", 
     "rail_low_001", "base_low_001", "arm_low_001", "arm2_low_001", "fingerl_low_001", "fingerr_low_001" // Mechanic arm
    }
);
std::map<std::string, Material*> TextureLoader::MATERIALS({
    {"rock", new Material(glm::vec3(1.0), glm::vec3(0.0), glm::vec3(1.0), 0.5)},
    {"tree", new Material(glm::vec3(0.0, 0.5, 0.0), glm::vec3(0.0), glm::vec3(0.0, 0.6, 0.0), 0.1)},
    {"water", new Material(glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0,0.0, 1.0), glm::vec3(1.0), 0.5)},
    {"plane", new Material(glm::vec3(0.2, 0.04, 0.0), glm::vec3(0.1, 0.05, 0.01), glm::vec3(1.0), 0.5)}
});

TextureLoader::TextureLoader()
{    
    for(std::string texname : TextureLoader::TEXTURED_NODE) {
        Material *m;
        std::vector<Texture*> texs;
        if (MATERIALS.find(texname) != MATERIALS.end())
            continue;
        else {
        
            m = new Material(glm::vec3(1.0), glm::vec3(1.0), glm::vec3(1.0), 0.1);
            
            //Load diffuse texture
            Texture* texture = Texture::fromFile((texname+"_base.png"), "textures", Texture::Diffuse);
            if(texture)
                texs.push_back(texture);
            else
                DEBUG(Debug::Error, "Cannot load texture %s\n", (texname+"_base.png").c_str());
            
            //Load Normals texture
            texture = Texture::fromFile((texname+"_normal.png"), "textures", Texture::Normal);
            if(texture)
                texs.push_back(texture);
            else
                DEBUG(Debug::Error, "Cannot load texture %s\n", (texname+"_normal.png").c_str());
            
            //Load Normals texture
            texture = Texture::fromFile((texname+"_height.png"), "textures", Texture::Height);
            if (texture)
                texs.push_back(texture);
            else
                DEBUG(Debug::Error, "Cannot load texture %s\n", (texname+"_height.png").c_str());
            
            m->setTextures(texs);
        }
        
        MATERIALS.insert(std::pair<std::string, Material*>(texname, m));
        
    }
    
    
}

void TextureLoader::loadTextures(Node *root) {
    
    //Get root node
    std::multimap<std::string, Drawable*> children = root->children();
    int boatid = 0;
    
    for (auto child: children){
        if(dynamic_cast<Mesh*>(child.second)) {
            Mesh* c = (Mesh*)child.second;
            
            std::string cval = root->name();
            std::transform(cval.begin(), cval.end(), cval.begin(), ::tolower);
                        
            auto m = std::find_if(MATERIALS.begin(), MATERIALS.end(), 
                           [=](const std::pair<std::string, Material*> & t){
                               return cval.find(t.first.c_str()) != std::string::npos;
                           }
            );
            
            if(m != MATERIALS.end()) //add texture to scene
                c->setMaterial(m->second);
            else 
                printf("Didnt find node with name: %s\n", cval.c_str());
        } else if(dynamic_cast<Node*>(child.second)) {
            Node* n = (Node*)child.second;
            loadTextures(n);            
        }
    }
    
}


