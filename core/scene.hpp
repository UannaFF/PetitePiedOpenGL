#ifndef SCENE_H
#define SCENE_H

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

#include <glm/glm.hpp>
#include <GL/glew.h>

#include <vector>
#include <map>
#include <string>

class Mesh;
class Texture;
class Camera;
class Light;


class Node;
class Shader;

class SceneException: public std::exception {
    public:
        SceneException(std::string s): _what(s){}
        const char* what() const noexcept { return _what.c_str(); }
        
    private:
        std::string _what;
};

class Scene {
    public:
        enum ShaderType {MaterialShader, LightShader};
    
        Scene();
        
        inline Mesh* getMesh(unsigned int i){ return _models[i]; }
        
        void addMesh(Mesh* m);
        inline void addTexture(Texture* t){ _textures.push_back(t); }
        inline void setCamera(Camera* c){ _camera = c; }
        inline void setRootNode(Node* n){ _main_node = n; }
        
        inline Node* rootNode(Node* n){ return _main_node; }
        Node* findNode(std::string n) const;
        
        inline void setShader(Shader* s, ShaderType t = MaterialShader) {
            _shaders.insert(std::pair<ShaderType, Shader*>(t, s));
        }
        inline Shader* defaultShader(ShaderType t = MaterialShader) {
            auto it = _shaders.find(t);
            if (it == _shaders.end()) return nullptr;
            return it->second;
        }
        
        void render();
        void displayNodeTree();
        
        static Scene* fromOBJ(std::string path);
        
        
    private:
        std::vector<Mesh*> _models;
        std::vector<Texture*> _textures;
        std::map<ShaderType, Shader*> _shaders;
        Node* _main_node;
        Camera* _camera;
        
        void _parseNode(Node* current, aiNode ** children, unsigned int nb_child);
        
};

class Node {
    public:
        Node(std::string name, Mesh* m, glm::mat4 transformation, Scene* scene, Node* parent = nullptr);
        
        inline glm::mat4 applyTransformation(GLuint model_vert, glm::mat4 localTransform = glm::mat4(1.f)){
            glm::mat4 t = localTransform * _transformation;
            glUniformMatrix4fv(model_vert, 1, GL_FALSE, &t[0][0]);
            return t;
        }
        
        void draw(glm::mat4 localTransform = glm::mat4(1.f));
        
        void dump(int level = 0);
        Node* find(std::string);
        
        inline void addChild(std::string i, Node* n) { _children.insert(std::pair<std::string, Node*>(i, n)); }
        
        inline Scene* scene() const     { return _scene; }
        inline Mesh* mesh() const { return _mesh; }
    private:
        Mesh* _mesh;
        std::map<std::string, Node*> _children;
        
        std::string _name;
        
        glm::mat4 _transformation;
        
        
        Node* _parent;
        Scene* _scene;
};

glm::mat4 aiMatrix4x4toglmMat4(aiMatrix4x4t<float>& ai_mat);
glm::vec3 aiColor3DtoglmVec3(aiColor3D& ai_col);
glm::vec3 aiVector3DtoglmVec3(aiVector3D& ai_vec);

#endif
