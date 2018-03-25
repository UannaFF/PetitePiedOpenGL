// Include standard headers
#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include <glm/glm.hpp>

#include "core/window.hpp"
#include "core/common.hpp"
#include "core/camera.hpp"

using namespace glm;

int main(int argc, char** argv){

	// Initialise GLFW
	if( !glfwInit() )
	{
	    std::cerr << "Failed to initialize GLFW\n";
	    return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 16); // 16x antialiasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL 

	// Open a window and create its OpenGL context
	
        try {
                Window window(1024, 768, "Petit Pied");
                window.initialise();
                
                ControlableCamera mainCamera;
                window.setCamera(mainCamera);

                do{
                    // Draw nothing, see you in tutorial 2 !
                    mainCamera.updateFromMouse();

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
