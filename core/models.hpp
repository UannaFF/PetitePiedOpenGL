#ifndef MODELS_H
#define MODELS_H


#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <map>

class Texture;
class Shader;
class Material;
class Joint;
class NodeAnimator;

typedef struct VertexWeight {
    uint vertex_id;
    float weight;
} vertex_weight_t;


class Bone {
    public:
        Bone(glm::mat4 offset): _offset(offset), _weights(), _frag(0) {}
        
        void dumpToBuffer(std::vector<int>& vertex_buff, std::vector<float>& weight_buff);
        
        inline int size() const { return _weights.size(); }
        
        inline void addWeight(uint vertext_id, float weight) {
            _weights.insert(std::make_pair(vertext_id, weight));
        }
        
        inline void frag_id(GLint f) { _frag = f; }
        inline GLint frag_id() { return _frag; }
        
        void setTransformation(const glm::mat4& mat);
        
        inline void id(uint i) { _boneid = i; }
        inline uint id() const { return _boneid; }
        
        static int LAST_ID;
    private:
        GLint _frag;
        uint _boneid;
        glm::mat4 _offset;
        std::map<uint, float> _weights;
        
};


class Mesh {
    public:
        Mesh();
        ~Mesh();
        void setVertex(std::vector<GLfloat> vertex);
        void setUV(std::vector<GLfloat> uv);
        void setNormal(std::vector<GLfloat> normal);
        void setIndice(std::vector<unsigned short> normal);
        void setBones(std::vector<Bone*> b, Shader* s);
        
        inline void setMode(GLenum m){_mode = m; }
        inline GLenum mode() const { return _mode; }
        
        inline void bind() const { return glBindVertexArray(_vertex_array_id); }
        
        virtual void draw(Shader* usedShader);
        
        inline void setMaterial(Material* m) { _material = m; }
        

    private:
        GLuint _vertex_array_id;
        
        GLuint _vertexbuffer;
        GLuint _uvbuffer;
        GLuint _normal;
        GLuint _indice;
        GLuint _bones_id;
        GLuint _weight;
        
        Material* _material;
        
        int _len_points;
        
        GLenum _mode;
        
        //~ std::vector<Bone*> _bones;
};
#endif
