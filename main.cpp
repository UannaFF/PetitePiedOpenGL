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
#include "core/scene.hpp"

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
            window.hideCursor();
            window.centerCursor();
            
            ControlableCamera mainCamera;
            window.setCamera(mainCamera);
            
            glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

            // Enable depth test
            glEnable(GL_DEPTH_TEST);
            // Accept fragment if it closer to the camera than the former one
            glDepthFunc(GL_LESS); 
            // Cull triangles which normal is not towards the camera
            glEnable(GL_CULL_FACE);

            std::vector<VertexArray*> models = VertexArray::fromOBJ("Environement.obj");
            Scene* scene = Scene::fromOBJ("Environement.dae");
            scene->displayNodeTree();
            
            // Create and compile our GLSL program from the shaders
            Shader* shader = Shader::fromFiles( "StandardShading.vertexshader", "StandardShading.fragmentshader" );

            shader->use();

            // Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
            mainCamera.setProjectionMatrix(glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f));
            // Camera matrix
            mainCamera.bindView(shader->getUniformLocation("V"));
            mainCamera.setViewMatrix(glm::lookAt(
                                        glm::vec3(20,15,0), // Camera in World Space
                                        glm::vec3(1,1,1), // and looks at the origin
                                        glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
                                   ));
            //~ mainCamera.setViewMatrix(glm::mat4(1.f));

            // Load the texture using any two methods
            //~ Texture* texture = Texture::fromBitmap("uvtemplate.bmp");
            //~ Texture* texture = Texture::fromDDS("uvmap.DDS");
            
            //~ if (!texture)
                //~ throw new OpenGLException("Cannot create the texture");
            
            // Get a handle for our "myTextureSampler" uniform
            GLuint TextureID  = shader->getUniformLocation("myTextureSampler");

            // Get a handle for our "LightPosition" uniform
            GLuint LightID = shader->getUniformLocation("LightPosition_worldspace");
                
            std::cout << std::endl;
            
            do{ 
                // Clear the screen
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                // Use our shader
                shader->use();
                                   
                // Update tarball from mouse and keyboard
                mainCamera.updateFromMouse();
                glm::mat4 MVP = mainCamera.projectionMatrix() * mainCamera.viewMatrix() * glm::mat4(1.f); // Remember, matrix multiplication is the other way around
                //~ std::cout << "V: " << std::endl << mainCamera.viewMatrix() << std::endl;
                
                
                // Send our transformation to the currently bound shader, 
                // in the "MVP" uniform
                glUniformMatrix4fv(shader->getUniformLocation("MVP"), 1, GL_FALSE, &MVP[0][0]);
                mainCamera.updateView();
                
                glm::vec3 lightPos = glm::vec3(4,4,4);
                glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);

                // Bind and Set our "myTextureSampler" sampler to use Texture
                //~ texture->set(TextureID);
                
                //~ scene->render(shader->getUniformLocation("M"));
                for (VertexArray* v: models){
                    v->bind();
                    v->draw();
                }

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
            //~ delete texture;
            
    } catch (OpenGLException* e){
            std::cout << "OpenGL exception: " << e->what() << std::endl;
            glfwTerminate();
            return EXIT_FAILURE;
    
    }
    
    return EXIT_SUCCESS;
}
