#include "utils.hpp"
#include "../core/shader.hpp"
#include <vector>

ReferenceMarker::ReferenceMarker(Shader* s):
    Mesh(s, new VertexArray, std::vector<Bone*>())
{
    std::vector<float> pos_color({0, 0, 0,
                                  3, 0, 0,
                                  0, 0, 0,
                                  0, 3, 0,
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

TextureLoader::TextureLoader():
    _texnames({"island", "boat_barriere", "rock", "tree", "water", "group16681", "plane", "fingerl_low_001", "fingerr_low_001","diplo_node"})
{
    _materials = std::map<std::string, Material*>();
    def = new Material(glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 1.0, 0.0), 0.5);
    
    
    for(std::string texname : _texnames) {
        Material *m;
        std::vector<Texture*> texs;
        printf("texnq,e: %s\n",texname.c_str());
        if (texname.compare("tree") == 0) {
            
            m = new Material(glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 1.0, 0.0), 0.5);
        } else if(texname.compare("rock") == 0) {
            m = new Material(glm::vec3(1.0), glm::vec3(0.0), glm::vec3(1.0), 0.5);
        }else if(texname.compare("water") == 0){
            m = new Material(glm::vec3(0.0,0.0, 1.0), glm::vec3(0.0,0.0, 1.0), glm::vec3(1.0), 0.1);
        } else {
        
            m = new Material(glm::vec3(1.0), glm::vec3(1.0), glm::vec3(1.0), 0.1);
            
            Texture* texture;
            
            //auto it = Texture::LOADED.find(str.C_Str());
            
            //Load diffuse texture
            texture = Texture::fromFile((texname+"_base.png"), "res/textures");
            if(texture) {
                texture->type(Texture::Diffuse);
                texs.push_back(texture);
            }
            
            //Load Normals texture
            texture = Texture::fromFile((texname+"_normal.png"), "res/textures");
            if(texture) {
                texture->type(Texture::Normal);
                texs.push_back(texture);
            }
            
            //Load Normals texture
            /*texture = Texture::fromFile((texname+"_height.png"), "res/textures");
            if(texture) {
                texture->type(Texture::Normal);
                texs.push_back(texture);
            }*/
            
            m->setTextures(texs);
        }
        
        _materials.insert(std::pair<std::string, Material*>(texname.c_str(), m));
        
    }
    
    
}

TextureLoader::~TextureLoader() {
    
}

void TextureLoader::loadTextures(Node *root) {
    
    //Get root node
    std::multimap<std::string, Drawable*> children = root->children();
    
    for (auto child: children){
        if(dynamic_cast<Mesh*>(child.second)) {
            Mesh* c = (Mesh*)child.second;
            
            //~ auto m = _materials.find(root->name().c_str());
            std::string cval = root->name();
            std::transform(cval.begin(), cval.end(), cval.begin(), ::tolower);
            auto m = std::find_if(_materials.begin(), _materials.end(), 
               [cval](const std::pair<std::string, Material*> & t) -> bool {
                   return cval.find(t.first.c_str()) != std::string::npos;
               }
            );
            
            if(m != _materials.end()) { //add texture to scene
                c->setMaterial(m->second);
                printf("FOUND node zith name: %s\n", cval.c_str());
            } else {
                printf("Didnt find node zith name: %s\n", cval.c_str());
                c->setMaterial(def);}
        } else if(dynamic_cast<Node*>(child.second)) {
            Node* n = (Node*)child.second;
            loadTextures(n);
            
        }
    }
    
}


