#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <GL/glew.h>

class Shader {
    public:
        Shader(std::string vertex, std::string fragment);
        ~Shader();
        
        inline GLuint getUniformLocation(const char* id) { return glGetUniformLocation(_programe_id, id); }
        inline void use() { return glUseProgram(_programe_id); }
        
        static Shader* fromFiles(std::string vertex_path, std::string fragment_path);
    private:
        GLuint _programe_id;
        
        
};

#endif
