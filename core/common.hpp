#ifndef COMMON_H
#define COMMON_H

#include <string>
#include <iostream>
#include <iomanip>
#include <glm/glm.hpp>

#include <GL/glew.h>
#include <GL/glew.h>

#define DEBUG(priority,format,args...)                                 \
                 if (priority > Info)                                  \
                    fprintf(stderr, format, ## args);                  \

class OpenGLException: public std::exception {
    public:
        OpenGLException(std::string error_msg, GLenum err): _msg(error_msg) {
            if (err)
                _msg += ": "+ std::string((const char*)glewGetErrorString(err)) + " ("+std::to_string(err)+")";
        }
        const char* what() const noexcept {
            return _msg.c_str();
        }
    private:
        std::string _msg;
};

class Drawable {
    public:
        virtual void draw(glm::mat4 projection, glm::mat4 view, glm::mat4 model)=0;
        virtual void dump(int level)=0;
};

std::ostream& operator<<(std::ostream& cout, const glm::mat4& m);
std::ostream& operator<<(std::ostream& cout, const glm::mat4& m);

namespace Debug {
    enum Level {VERBOSE, INFO, WARNING, ERROR};
    
    inline void CheckOpenGLError(std::string label){   
        GLenum err;     
        while ((err = glGetError()) != GL_NO_ERROR)
            throw new OpenGLException(label, err);
    }
}

#endif
