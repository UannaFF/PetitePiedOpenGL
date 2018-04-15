#ifndef MODELS_H
#define MODELS_H

#include "common.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <map>

class Shader;
class Material;
class Node;

#define GL_LAYOUT_VERTEXARRAY 0
#define GL_LAYOUT_UV 1
#define GL_LAYOUT_NORMAL 2
#define GL_LAYOUT_BONES 3
#define GL_LAYOUT_WEIGHT 4
#define GL_LAYOUT_TANGENT 5
#define GL_LAYOUT_BITANGENT 6

class Bone {
    public:
        Bone(glm::mat4 offset): _offset(offset), _weights(), _frag(0), _node(nullptr) {}
        
        void dumpToBuffer(std::vector<int>& vertex_buff, std::vector<float>& weight_buff);
        
        inline int size() const { return _weights.size(); }
        
        inline void addWeight(uint vertext_id, float weight) {
            _weights.insert(std::make_pair(vertext_id, weight));
        }
        
        inline void attach(Node* n) { _node = n; }
        inline Node* node() { return _node; }
        
        glm::mat4 transformation() const ;
        
        inline void id(uint i) { _boneid = i; }
        inline uint id() const { return _boneid; }
        
        static int LAST_ID;
    private:
        GLint _frag;
        uint _boneid;
        glm::mat4 _offset;
        std::map<uint, float> _weights;
        
        Node* _node;
        
        void normalize();
        
};


class VertexArray {
    public:
        VertexArray();
        ~VertexArray();
        void setVertex(std::vector<GLfloat> vertex);
        void setUV(std::vector<GLfloat> uv);
        void setNormal(std::vector<GLfloat> normal);
        void setIndice(std::vector<unsigned short> normal);
        void setBones(std::vector<Bone*> b, Shader* s);
        void setTangents(std::vector<GLfloat> tangents);
        void setBiTangents(std::vector<GLfloat> bitangents);
        
        void computeTangentBasis(std::vector<GLfloat>& v, std::vector<GLfloat>& u, std::vector<GLfloat>& n);
        
        virtual void draw(GLint primitive);    

    private:
        GLuint _vertex_array_id;
        
        GLuint _vertexbuffer, _uvbuffer, _normal, _indice, _bones_id, _weight, _tangent, _bitangent;
        
        int _len_points;
        
        //~ std::vector<Bone*> _bones;
};

class Mesh : public Drawable {
    public:
        Mesh(Shader* s, VertexArray* va, std::vector<Bone*> bones):_shader(s), _vao(va), _bones(bones), _material(nullptr) {}
        ~Mesh();
        
        void draw(glm::mat4 projection, glm::mat4 view, glm::mat4 model);
        
        void dump(int level){
            std::cout << std::setw(level * 2) << "Mesh with " << _bones.size() << " bones." << std::endl;
        }
        
        inline void setMaterial(Material* m) { _material = m; }  
          
        inline VertexArray* VAO() const { return _vao; }    
        inline void setVAO(VertexArray* va) { _vao = va; }   
          
        inline const std::vector<Bone*>& bones() const { return _bones; }  
        
        static GLint VA_PRIMITIVE; //= GL.GL_TRIANGLES
        
    private:    
        Material* _material;
        
        std::vector<Bone*> _bones;
        Shader* _shader;
        VertexArray* _vao;
};

class Skybox : public Mesh {
    public:
        Skybox(Shader* s);

        //Define a big cube
    
};
#endif
