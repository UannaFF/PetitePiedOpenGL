#ifndef COMMON_H
#define COMMON_H

class OpenGLException: public std::exception {
    public:
        OpenGLException(std::string error_msg): _msg(error_msg) {}
        const char* what() const noexcept {
            return _msg.c_str();
        }
    private:
        std::string _msg;
};

#endif
