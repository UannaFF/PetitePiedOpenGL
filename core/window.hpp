#ifndef WINDOW_H
#define WINDOW_H

extern "C" {
#include <GL/glew.h>
#include <GLFW/glfw3.h>
}
#include <exception>
#include <string>

class Camera;

class Window {
    public:
        Window(int width, int height, std::string title);
        bool initialise();
        void preDrawingEvent();
        void postDrawingEvent();
        void hideCursor();
        void centerCursor();
        
        inline int keyStatus(int) const {
            return glfwGetKey(_gl_window, GLFW_KEY_ESCAPE );
        }
        
        inline int getKey(int key) const {
            return glfwGetKey(_gl_window, key );
        }
        
        inline void swap() {
            glfwSwapBuffers(_gl_window);
        }
        
        inline int shouldClose() const {
            return glfwWindowShouldClose(_gl_window);
        }
        
        inline GLFWwindow* internal() {
            return _gl_window;
        }
        
        inline float ratio() { return width() / height(); }
        
        inline int width() {
            int width, height;
            glfwGetWindowSize(_gl_window, &width, &height);
            return width;
        }
        
        inline int height() {
            int width, height;
            glfwGetWindowSize(_gl_window, &width, &height);
            return height;
        }
        
    private:
        GLFWwindow* _gl_window;
};

#endif
