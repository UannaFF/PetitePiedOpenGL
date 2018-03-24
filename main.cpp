// Include standard headers
#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include <glm/glm.hpp>

#include "core/window.hpp"

using namespace glm;

int main(){

	// Initialise GLFW
	if( !glfwInit() )
	{
	    fprintf( stderr, "Failed to initialize GLFW\n" );
	    return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL 

	// Open a window and create its OpenGL context
	
        try {
                Window window(1024, 768, "Petit Pied");
                window.initialise();

                do{
                    // Draw nothing, see you in tutorial 2 !

                    // Swap buffers
                    window.postDrawingEvent();
                }

                // Check if the ESC key was pressed or the window was closed
                while( window.keyStatus(GLFW_KEY_ESCAPE) != GLFW_PRESS &&
                        !window.shouldClose());
                
        } catch (OpenGLException* e){
                std::cout << "OpenGL exception: " << e->what();
                glfwTerminate();
                return EXIT_FAILURE;
	    
        }
        
        return EXIT_SUCCESS;
}
