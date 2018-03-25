#include "window.hpp"
#include "common.hpp"
#include "camera.hpp"
    
Window::Window(int width, int height, std::string title):
    _gl_window(glfwCreateWindow(width, height, title.c_str(), NULL, NULL)) {
    
    if( _gl_window == NULL )
	    throw new OpenGLException("Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
        
}

void Window::setCamera(Camera& c) {
    _active_camera = &c;
    c.setParent(this);
}

bool Window::initialise(){
    glfwMakeContextCurrent(_gl_window); // Initialize GLEW
	glewExperimental=true; // Needed in core profile
	if (glewInit() != GLEW_OK)
	    throw new OpenGLException("Failed to initialize GLEW\n");
	
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
}

void Window::postDrawingEvent(){
    glfwSwapBuffers(_gl_window);
    glfwPollEvents();
}
        
