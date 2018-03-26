#ifndef COMMON_H
#define COMMON_H

#include <string>
#include <iostream>
#include <iomanip>


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

std::ostream& operator<<(std::ostream& cout, const glm::mat4& m){
    cout << std::setw(6) << '[';
    for (int i = 0; i < 4; i++){
        if (i)
            cout << ' ';
        cout << m[i].x << ", " << m[i].y << ", " << m[i].z << ", " << m[i].w;
        if (i != 3)
            cout << std::endl << std::setw(6);
    }
    cout << ']' << std::endl;
    return cout;
}

#endif
