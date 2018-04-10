#ifndef MODELS_H
#define MODELS_H

#include "common.hpp"

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

#define GL_LAYOUT_VERTEXARRAY 0
#define GL_LAYOUT_UV 1
#define GL_LAYOUT_NORMAL 2
//~ #define GL_LAYOUT_INDICE 0
#define GL_LAYOUT_BONES 3
#define GL_LAYOUT_WEIGHT 4

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
        
        void normalize();
        
};


class VertexArray {
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
        
        virtual void draw(GLint primitive);
        
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
        Shader* _shader;
        
        int _len_points;
        
        GLenum _mode;
        
        //~ std::vector<Bone*> _bones;
};

class Mesh : public Drawable {
    public:
        Mesh(Shader* s):_shader(s) {}
        
        void draw(glm::mat4 projection, glm::mat4 view, glm::mat4 model){
            _shader->use();

            // setup camera geometry parameters
            _shader->setMat4("projection", projection, GL_TRUE);
            _shader->setMat4("view", view, GL_TRUE);

            // bone world transform matrices need to be passed for skinning
            for (Bone* b: _bones){
                bone_matrix = node.world_transform @ self.bone_offsets[bone_id]

                bone_loc = GL.glGetUniformLocation(shid, 'boneMatrix[%d]' % bone_id)
                GL.glUniformMatrix4fv(bone_loc, 1, True, bone_matrix)

            // draw mesh vertex array
            _vao->draw(VA_PRIMITIVE)

            // leave with clean OpenGL state, to make it easier to detect problems
            __shader->deuse(0);
        }
    private:
        GLint VA_PRIMITIVE; //= GL.GL_TRIANGLES
    
        std::vector<Bind*> _bones;
        Shader* _shader;
        VertexArray* _vao;
};

class Skybox : public Mesh {
    public:
        Skybox();
        void setEverything();
        ~Skybox();

        //Define a big cube
    
};
#endif
