#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>

class Shader {
    public:
        Shader(std::string vertex, std::string fragment);
        ~Shader();
        
        inline GLuint getUniformLocation(std::string id) { return getUniformLocation(id.c_str()); }
        inline GLuint getUniformLocation(const char* id) { return glGetUniformLocation(_programe_id, id); }
        inline void use() { return glUseProgram(_programe_id); }
        
        inline void setVec3(const std::string &name, const glm::vec3 &value) const { 
            glUniform3fv(glGetUniformLocation(_programe_id, name.c_str()), 1, &value[0]); 
        }        
        inline void setVec3(const std::string &name, float x, float y, float z) const { 
            glUniform3f(glGetUniformLocation(_programe_id, name.c_str()), x, y, z); 
        }
        inline void setMat4(const std::string &name, const glm::mat4 &mat) const {
            glUniformMatrix4fv(glGetUniformLocation(_programe_id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
        }

        
        static Shader* fromFiles(std::string vertex_path, std::string fragment_path);
        GLuint _programe_id;
    private:
        
        
        
};

#endif
