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
    
    std::string scene_file = "res/rebo.obj";
    if (argc == 2)
        scene_file = std::string(argv[1]);

	glfwWindowHint(GLFW_SAMPLES, 16); // 16x antialiasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

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
            // Cull triangles which normal is not towards the camera
            //~ glEnable(GL_CULL_FACE);

            //~ std::vector<Mesh*> models = Mesh::fromOBJ("Environement.obj");
            Scene* scene = Scene::import(scene_file, Shader::fromFiles( "shaders/vertexshader_material.glsl", "shaders/fragment_material.glsl"));
            scene->displayNodeTree();
            
            // Create and compile our GLSL program from the shaders
            //~ Shader* shader = Shader::fromFiles( "shaders/vertexshader_material.glsl", "shaders/fragment_material.glsl" );
            //~ Shader* light_shader = Shader::fromFiles( "shaders/vertexshader_light.glsl", "shaders/fragment_light.glsl" );
            //~ Shader* shader = Shader::fromFiles( "shaders/StandardShading.vertexshader", "shaders/StandardShading.fragmentshader" );
            //~ scene->setShader(shader);

            scene->setSkybox("skybox_sky", "shaders/Skyboxshadingvertex.glsl","shaders/Skyboxshadingfragment.glsl" );

            scene->defaultShader()->use();
            
            mainCamera.bindView(scene->defaultShader()->getUniformLocation("view"));
            mainCamera.bindProjection(scene->defaultShader()->getUniformLocation("projection"));
            
            // Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
            mainCamera.setProjectionMatrix(glm::perspective(glm::radians(45.0f), window.ratio(), 0.1f, 100.0f));
            // Camera matrix
            mainCamera.setViewMatrix(glm::lookAt(
                                        glm::vec3(20,15,0), // Camera in World Space
                                        glm::vec3(1,1,1), // and looks at the origin
                                        glm::vec3(0,-1,0)  // Head is up (set to 0,-1,0 to look upside-down)
                                   ));


            
            scene->skyboxShader()->use();
            glm::mat4 projection = mainCamera.projectionMatrix();
            glm::mat4 view = mainCamera.viewMatrix();
            glUniformMatrix4fv(scene->skyboxShader()->getUniformLocation("projection"), 
                1, GL_FALSE, &projection[0][0]);
            glUniformMatrix4fv(scene->skyboxShader()->getUniformLocation("view"), 
                1, GL_FALSE, &view[0][0]);


            //scene->defaultShader()->use();

            // Load the texture using any two methods
            //~ Texture* texture = Texture::fromBitmap("uvtemplate.bmp");
            //~ Texture* texture = Texture::fromDDS("uvmap.DDS");
            
            //~ if (!texture)
                //~ throw new OpenGLException("Cannot create the texture");
            
            // Get a handle for our "myTextureSampler" uniform
            //~ GLuint TextureID  = scene->defaultShader()->getUniformLocation("myTextureSampler");

            // Get a handle for our "LightPosition" uniform
            //~ GLuint LightID = scene->defaultShader()->getUniformLocation("LightPosition_worldspace");

                
            //~ std::cout << std::endl;
                
            glm::vec3 lightPos = glm::vec3(4,4,4);
            
            
            window.hideCursor();
            window.centerCursor();
            
            scene->playAnimation(0);
            
            do{ 
                // Clear the screen
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                        
                scene->defaultShader()->setVec3("light.position", lightPos);
                //~ scene->defaultShader()->getUniformLocation("viewPos", camera.Position);


                // light properties
                glm::vec3 lightColor;
                lightColor.x = sin(glfwGetTime() * 2.0f);
                lightColor.y = sin(glfwGetTime() * 0.7f);
                lightColor.z = sin(glfwGetTime() * 1.3f);
                glm::vec3 diffuseColor = lightColor   * glm::vec3(0.5f); // decrease the influence
                glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f); // low influence
                scene->defaultShader()->setVec3("light.ambient", ambientColor);
                scene->defaultShader()->setVec3("light.diffuse", diffuseColor);
                scene->defaultShader()->setVec3("light.specular", 1.0f, 1.0f, 1.0f);
    
                // Update P and V from mouse and keyboard
                mainCamera.updateFromMouse();
                
                //~ glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);

                // Bind and Set our "myTextureSampler" sampler to use Texture
                //~ texture->set(TextureID);
                scene->skyboxShader()->use();
                glm::mat4 projection = mainCamera.projectionMatrix();
            glm::mat4 view = mainCamera.viewMatrix();
                glUniformMatrix4fv(scene->skyboxShader()->getUniformLocation("projection"), 
                    1, GL_FALSE, &projection[0][0]);
                glUniformMatrix4fv(scene->skyboxShader()->getUniformLocation("view"), 
                    1, GL_FALSE, &view[0][0]);
                
                scene->render();

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
            //~ delete shader;
            //~ delete texture;
            
    } catch (OpenGLException* e){
            std::cout << "OpenGL exception: " << e->what() << std::endl;
            glfwTerminate();
            return EXIT_FAILURE;
    
    } catch (SceneException* e){
            std::cout << "Scene exception: " << e->what() << std::endl;
            glfwTerminate();
            return EXIT_FAILURE;
    
    }
    glfwTerminate();
    
    return EXIT_SUCCESS;
}
