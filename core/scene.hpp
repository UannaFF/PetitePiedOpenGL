#ifndef SCENE_H
#define SCENE_H

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

#include <glm/glm.hpp>
#include <GL/glew.h>

#include <vector>
#include <string>

class VertexArray;
class Texture;
class Camera;


class Node;

class Scene {
    public:
        Scene();
        
        inline VertexArray* getMesh(unsigned int i){ return _models[i]; }
        
        inline void addMesh(VertexArray* m){ _models.push_back(m); }
        inline void addTexture(Texture* t){ _textures.push_back(t); }
        inline void setCamera(Camera* c){ _camera = c; }
        inline void setRootNode(Node* n){ _main_node = n; }
        
        void render(GLuint model_vert);
        void displayNodeTree();
        
        static Scene* fromOBJ(std::string path);
        
        
    private:
        std::vector<VertexArray*> _models;
        std::vector<Texture*> _textures;
        Node* _main_node;
        Camera* _camera;
        
        void _parseNode(Node* current, aiNode ** children, unsigned int nb_child);
        
};

class Node {
    public:
        Node(std::string name, std::vector<VertexArray*> vertexarray, glm::mat4 transformation, Scene* scene, Node* parent = nullptr);
        
        void draw(GLuint model_vert, glm::mat4 localTransform = glm::mat4(1.f));
        
        void dump(int level = 0);
        
        inline void addChild(Node* n) { _children.push_back(n); }
        
    private:
        std::vector<Node*> _children;
        
        std::string _name;
        std::vector<VertexArray*> _meshs;
        
        glm::mat4 _transformation;
        
        
        Node* _parent;
        Scene* _scene;
};

glm::mat4 aiMatrix4x4toglmMat4(aiMatrix4x4t<float>& ai_mat);

#endif
