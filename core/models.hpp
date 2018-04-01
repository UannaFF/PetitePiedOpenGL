#ifndef MODELS_H
#define MODELS_H


#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <vector>
#include <string>

class Texture;
class Shader;
class Material;
class Joint;
class NodeAnimator;

typedef struct VertexWeight {
    uint vertex_id;
    float weight;
} vertex_weight_t;


class Mesh {
    public:
        Mesh();
        ~Mesh();
        void setVertex(std::vector<GLfloat> vertex);
        void setUV(std::vector<GLfloat> uv);
        void setNormal(std::vector<GLfloat> normal);
        void setIndice(std::vector<unsigned short> normal);
        
        inline void setMode(GLenum m){_mode = m; }
        inline GLenum mode() const { return _mode; }
        
        inline void bind() const { return glBindVertexArray(_vertex_array_id); }
        
        virtual void draw(Shader* usedShader);
        
        inline void setMaterial(Material* m) { _material = m; }
        
        static std::vector<Mesh*> fromOBJ(std::string path);
        
        std::vector<glm::mat4> getJointTransforms();
        
        inline void setAnimator(NodeAnimator* a) { _animator = a; }
        void setRootBone(Joint* j);

        void addJointsToArray(Joint* headJoint, std::vector<glm::mat4>& jointMatrices);
        
        inline Joint* rootBone() const { return _rootBone; }
    private:
        GLuint _vertex_array_id;
        GLuint _uvbuffer;
        GLuint _vertexbuffer;
        GLuint _normal;
        GLuint _indice;
        GLuint _bones_id;
        
        Material* _material;
        
        int _len_points;
        
        GLenum _mode;
        
        Joint* _rootBone;
        int _bonesCount;
        NodeAnimator* _animator;
};
#endif
