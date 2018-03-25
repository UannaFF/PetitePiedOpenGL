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
#include "core/shader.hpp"
#include "core/texture.hpp"
#include "core/models.hpp"

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
                
                glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

                // Enable depth test
                glEnable(GL_DEPTH_TEST);
                // Accept fragment if it closer to the camera than the former one
                glDepthFunc(GL_LESS); 

                VertexArray model;
                model.bind();

                // Create and compile our GLSL program from the shaders
                Shader* shader = Shader::fromFiles( "TransformVertexShader.vertexshader", "TextureFragmentShader.fragmentshader" );

                // Get a handle for our "MVP" uniform
                GLuint MatrixID = shader->getUniformLocation("MVP");

                // Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
                mainCamera.setProjectionMatrix(glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f));
                // Camera matrix
                mainCamera.setViewMatrix(glm::lookAt(
                                            glm::vec3(4,3,3), // Camera is at (4,3,3), in World Space
                                            glm::vec3(1,0,0), // and looks at the origin
                                            glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
                                       ));
                // Model matrix : an identity matrix (model will be at the origin)
                glm::mat4 Model      = glm::mat4(1.0f);
                // Our ModelViewProjection : multiplication of our 3 matrices
                glm::mat4 MVP;

                // Load the texture using any two methods
                //GLuint Texture = loadBMP_custom("uvtemplate.bmp");
                Texture* texture = Texture::fromDDS("uvtemplate.DDS");
                
                if (!texture)
                    throw new OpenGLException("Cannot create the texture");
                
                // Get a handle for our "myTextureSampler" uniform
                GLuint TextureID  = shader->getUniformLocation("myTextureSampler");

                // Our vertices. Tree consecutive floats give a 3D vertex; Three consecutive vertices give a triangle.
                // A cube has 6 faces with 2 triangles each, so this makes 6*2=12 triangles, and 12*3 vertices
                model.setVertex({ 
                    -1.0f,-1.0f,-1.0f,
                    -1.0f,-1.0f, 1.0f,
                    -1.0f, 1.0f, 1.0f,
                     1.0f, 1.0f,-1.0f,
                    -1.0f,-1.0f,-1.0f,
                    -1.0f, 1.0f,-1.0f,
                     1.0f,-1.0f, 1.0f,
                    -1.0f,-1.0f,-1.0f,
                     1.0f,-1.0f,-1.0f,
                     1.0f, 1.0f,-1.0f,
                     1.0f,-1.0f,-1.0f,
                    -1.0f,-1.0f,-1.0f,
                    -1.0f,-1.0f,-1.0f,
                    -1.0f, 1.0f, 1.0f,
                    -1.0f, 1.0f,-1.0f,
                     1.0f,-1.0f, 1.0f,
                    -1.0f,-1.0f, 1.0f,
                    -1.0f,-1.0f,-1.0f,
                    -1.0f, 1.0f, 1.0f,
                    -1.0f,-1.0f, 1.0f,
                     1.0f,-1.0f, 1.0f,
                     1.0f, 1.0f, 1.0f,
                     1.0f,-1.0f,-1.0f,
                     1.0f, 1.0f,-1.0f,
                     1.0f,-1.0f,-1.0f,
                     1.0f, 1.0f, 1.0f,
                     1.0f,-1.0f, 1.0f,
                     1.0f, 1.0f, 1.0f,
                     1.0f, 1.0f,-1.0f,
                    -1.0f, 1.0f,-1.0f,
                     1.0f, 1.0f, 1.0f,
                    -1.0f, 1.0f,-1.0f,
                    -1.0f, 1.0f, 1.0f,
                     1.0f, 1.0f, 1.0f,
                    -1.0f, 1.0f, 1.0f,
                     1.0f,-1.0f, 1.0f
                });

                // Two UV coordinatesfor each vertex. They were created with Blender.
                model.setUV({ 
                    0.000059f, 1.0f-0.000004f, 
                    0.000103f, 1.0f-0.336048f, 
                    0.335973f, 1.0f-0.335903f, 
                    1.000023f, 1.0f-0.000013f, 
                    0.667979f, 1.0f-0.335851f, 
                    0.999958f, 1.0f-0.336064f, 
                    0.667979f, 1.0f-0.335851f, 
                    0.336024f, 1.0f-0.671877f, 
                    0.667969f, 1.0f-0.671889f, 
                    1.000023f, 1.0f-0.000013f, 
                    0.668104f, 1.0f-0.000013f, 
                    0.667979f, 1.0f-0.335851f, 
                    0.000059f, 1.0f-0.000004f, 
                    0.335973f, 1.0f-0.335903f, 
                    0.336098f, 1.0f-0.000071f, 
                    0.667979f, 1.0f-0.335851f, 
                    0.335973f, 1.0f-0.335903f, 
                    0.336024f, 1.0f-0.671877f, 
                    1.000004f, 1.0f-0.671847f, 
                    0.999958f, 1.0f-0.336064f, 
                    0.667979f, 1.0f-0.335851f, 
                    0.668104f, 1.0f-0.000013f, 
                    0.335973f, 1.0f-0.335903f, 
                    0.667979f, 1.0f-0.335851f, 
                    0.335973f, 1.0f-0.335903f, 
                    0.668104f, 1.0f-0.000013f, 
                    0.336098f, 1.0f-0.000071f, 
                    0.000103f, 1.0f-0.336048f, 
                    0.000004f, 1.0f-0.671870f, 
                    0.336024f, 1.0f-0.671877f, 
                    0.000103f, 1.0f-0.336048f, 
                    0.336024f, 1.0f-0.671877f, 
                    0.335973f, 1.0f-0.335903f, 
                    0.667969f, 1.0f-0.671889f, 
                    1.000004f, 1.0f-0.671847f, 
                    0.667979f, 1.0f-0.335851f
                });
                
                do{
                    // Draw nothing, see you in tutorial 2 !
                    mainCamera.updateFromMouse();
                    MVP = mainCamera.projectionMatrix() * mainCamera.viewMatrix() * Model; // Remember, matrix multiplication is the other way around
                    
                    // Clear the screen
                    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                    // Use our shader
                    shader->use();
                    
                    // Send our transformation to the currently bound shader, 
                    // in the "MVP" uniform
                    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

                    // Bind and Set our "myTextureSampler" sampler to use Texture
                    texture->set(TextureID);
                    
                    model.draw();

                    // Swap buffers
                    window.swap();
                    glfwPollEvents();

                    // Swap buffers
                    window.postDrawingEvent();
                }

                // Check if the ESC key was pressed or the window was closed
                while( window.keyStatus(GLFW_KEY_ESCAPE) != GLFW_PRESS &&
                        !window.shouldClose());

                // Cleanup VBO and shader
                delete shader;
                delete texture;
                
        } catch (OpenGLException* e){
                std::cout << "OpenGL exception: " << e->what() << std::endl;
                glfwTerminate();
                return EXIT_FAILURE;
	    
        }
        
        return EXIT_SUCCESS;
}
