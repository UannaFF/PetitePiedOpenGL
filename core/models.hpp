#ifndef MODELS_H
#define MODELS_H


#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <vector>

class VertexArray {
    public:
        VertexArray();
        ~VertexArray();
        void setVertex(std::vector<GLfloat> vertex);
        void setUV(std::vector<GLfloat> uv);
        
        inline void setMode(GLenum m){_mode = m; }
        inline GLenum mode() const { return _mode; }
        
        inline void bind() const { return glBindVertexArray(_vertex_array_id); }
        
        void draw();
    private:
        GLuint _vertex_array_id;
        GLuint _uvbuffer;
        GLuint _vertexbuffer;
        
        int _len_points;
        
        GLenum _mode;
};
#endif
