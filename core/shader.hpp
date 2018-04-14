#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>

class ShaderException : public std::exception {};

class ShaderUniformNotFoundException : public ShaderException {
    public:
        ShaderUniformNotFoundException(std::string s): _what(s){}
        const char* what() const noexcept { return _what.c_str(); }
        
    private:
        std::string _what;
};

class Shader {
    public:
        Shader(std::string vertex, std::string fragment);
        ~Shader();
        
        GLuint getUniformLocation(std::string id);
        GLuint getUniformLocation(const char* id);
        
        void use();
        void deuse();
        
        void setVec3(const std::string &name, const glm::vec3 &value) const;
        void setVec3(const std::string &name, float x, float y, float z) const;
        void setMat4(const std::string &name, const glm::mat4 &mat, bool inverse = GL_FALSE) const;
        void setFloat(const std::string &name, float val) const;

        inline std::string name() const { return _name; }
        inline void name(std::string name) { _name = name; }
        
        static Shader* fromFiles(std::string vertex_path, std::string fragment_path);
    private:
        GLuint _programe_id;
        
        std::string _name;
        
        static GLuint SHADER_IN_USE;
        
        
        
};

class ShaderNotUseException : public ShaderException {
    public:
        ShaderNotUseException(const Shader* s): _who(s){}
        const char* what() const noexcept { return "Requested shader element is not in use"; }
        
    private:
        const Shader* _who;
};


#endif
