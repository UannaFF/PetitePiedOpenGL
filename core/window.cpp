#include "window.hpp"
#include "common.hpp"
#include "camera.hpp"

#include <iostream>
    
Window::Window(int width, int height, std::string title):
    _gl_window(glfwCreateWindow(width, height, title.c_str(), NULL, NULL)) {
    
    if( _gl_window == NULL )
	    throw new OpenGLException("Failed to open GLFW window\n", 0);
        
}

bool Window::initialise(){
    glfwMakeContextCurrent(_gl_window); // Initialize GLEW
    
	glewExperimental=true; // Needed in core profile
    GLenum err;
    
	if ((err = glewInit()) != GLEW_OK)
	    throw new OpenGLException("Failed to initialize GLEW", err);
            
    while ((err = glGetError()) != GL_NO_ERROR && err != GL_INVALID_ENUM)
        throw new OpenGLException("OpenGL error", err);
	

    std::cout << "OpenGL " << glGetString(GL_VERSION) << ", GLSL " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
  
    glfwSetInputMode(_gl_window, GLFW_STICKY_KEYS, GL_TRUE);
    
    return 0;
}

void Window::hideCursor(){
    glfwSetInputMode(_gl_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void Window::centerCursor(){
    glfwSetCursorPos(_gl_window, width()/2, height()/2);
}

void Window::preDrawingEvent(){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Window::postDrawingEvent(){
    glfwSwapBuffers(_gl_window);
    glfwPollEvents();
}
        
