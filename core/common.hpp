#ifndef COMMON_H
#define COMMON_H

#include <string>
#include <iostream>
#include <iomanip>
#include <glm/glm.hpp>


namespace Debug {
    enum Level {VERBOSE, INFO, WARNING, ERROR};
}

#define DEBUG(priority,format,args...)                                 \
                 fprintf(stderr, format, ## args);                     \

#define OBSELETE printf("OBESELETE\n");                                \
                 assert(false);                                        \

class OpenGLException: public std::exception {
    public:
        OpenGLException(std::string error_msg): _msg(error_msg) {}
        const char* what() const noexcept {
            return _msg.c_str();
        }
    private:
        std::string _msg;
};

class Drawable {
    public:
        virtual void draw(glm::mat4 projection, glm::mat4 view, glm::mat4 model)=0;
};

std::ostream& operator<<(std::ostream& cout, const glm::mat4& m);

#endif
