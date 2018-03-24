#ifndef WINDOW_H
#define WINDOW_H


#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <exception>
#include <string>

class OpenGLException: public std::exception {
    public:
        OpenGLException(std::string error_msg): _msg(error_msg) {}
        const char* what() const noexcept {
            return _msg.c_str();
        }
    private:
        std::string _msg;
};

class Window {
    public:
        Window(int width, int height, std::string title);
        bool initialise();
        void preDrawingEvent();
        void postDrawingEvent();
        
        inline int keyStatus(int) {
            return glfwGetKey(_gl_window, GLFW_KEY_ESCAPE );
        }
        
        inline int shouldClose() {
            return glfwWindowShouldClose(_gl_window);
        }
        
    private:
        GLFWwindow* _gl_window;
};

#endif
