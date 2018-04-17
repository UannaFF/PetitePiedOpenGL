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

#include "assets/utils.hpp"
#include "assets/world.hpp"

using namespace glm;
static int oldState = GLFW_RELEASE;

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
    
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	
    try {
            Window window(1024, 768, "Petit Pied");
            window.initialise();
            
            ControlableCamera mainCamera(&window);
            
            glClearColor(0.7f, 0.7f, 0.7f, 0.0f);

            // Enable depth test
            glEnable(GL_DEPTH_TEST);
            // Accept fragment if it closer to the camera than the former one
            glDepthFunc(GL_LESS); 
            // Cull triangles which normal is not towards the camera
            //~ glEnable(GL_CULL_FACE);

            //~ std::vector<Mesh*> models = Mesh::fromOBJ("Environement.obj");
            //~ Scene* scene = Scene::import(scene_file, Shader::fromFiles( "shaders/vertexshader_material.glsl", "shaders/fragment_material.glsl"));
            Scene* scene = DinoWorld::buildScene();
            
            //~ Camera mainCamera;
            scene->setCamera(&mainCamera);
            
            // Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
            mainCamera.setProjectionMatrix(glm::perspective(glm::radians(45.0f), window.ratio(), 0.1f, 100.0f));
            // Camera matrix
            mainCamera.setViewMatrix(glm::lookAt(
                                        glm::vec3(100,50,100), // Camera in World Space
                                        glm::vec3(1,1,1), // and looks at the origin
                                        glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
                                   ));

            // Marker
            /*Node* markerNode = new Node("MarkerNode", glm::mat4(1.f) * translation(0.f, 0.f, 1.f), scene, scene->rootNode());
            markerNode->addChild("markerMesh", new ReferenceMarker(Shader::fromFiles( "shaders/vertexshader_marker.glsl", "shaders/fragment_marker.glsl")));
            scene->rootNode()->addChild("marker", markerNode);*/
            
            // Skybox
            scene->setSkybox("skybox_sky", "shaders/Skyboxshadingvertex.glsl","shaders/Skyboxshadingfragment.glsl" );
            

            window.hideCursor();
            window.centerCursor();
            
            scene->playAnimation(0);
            glm::vec3 lightPos = glm::vec3(0,-2.7, 4);   
            
            float time_last_frame = glfwGetTime();
            DEBUG(Debug::Info, "\n");
            
            //test the loading textures<
            TextureLoader tl;
            tl.loadTextures(scene->rootNode());
            
            do{ 
                // Clear the screen
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                
                scene->defaultShader()->use();
                
                //~ if (glfwGetKey( window.internal(), GLFW_KEY_X ) == GLFW_PRESS)
                    //~ trans[0][3] += 0.05;
                //~ if (glfwGetKey( window.internal(), GLFW_KEY_W ) == GLFW_PRESS)
                    //~ trans[0][3] -= 0.05;
                //~ if (glfwGetKey( window.internal(), GLFW_KEY_Q ) == GLFW_PRESS)
                    //~ trans[1][3] += 0.05;
                //~ if (glfwGetKey( window.internal(), GLFW_KEY_D ) == GLFW_PRESS)
                    //~ trans[1][3] -= 0.05;
                //~ if (glfwGetKey( window.internal(), GLFW_KEY_U ) == GLFW_PRESS)
                    //~ trans[2][3] += 0.05;
                //~ if (glfwGetKey( window.internal(), GLFW_KEY_I ) == GLFW_PRESS)
                    //~ trans[2][3] -= 0.05;
                    
                //~ std::cout << trans[0][3] <<", " << trans[1][3] << ", " << trans[2][3] << std::endl;
                //~ boat_scene->rootNode()->setTransformation(boat_rotate_scale * trans);
                //~ scene->defaultShader()->setVec3("light.position", lightPos);
                    
                //~ glm::vec3 lightColor;
                //~ lightColor.x = sin(glfwGetTime() * 2.0f);
                //~ lightColor.y = sin(glfwGetTime() * 0.7f);
                //~ lightColor.z = sin(glfwGetTime() * 1.3f);
                //~ glm::vec3 diffuseColor = lightColor   * glm::vec3(0.5f); // decrease the influence
                //~ glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f); // low influence
                /*scene->defaultShader()->setVec3("light.ambient", 1.0f, 1.0f, 1.0f);
                scene->defaultShader()->setVec3("light.diffuse", 1.0f, 1.0f, 1.0f);
                scene->defaultShader()->setVec3("light.specular", 1.0f, 1.0f, 1.0f);
                */
                scene->defaultShader()->deuse();  
                
                //~ scene->defaultShader()->getUniformLocation("viewPos", camera.Position);


                // Update P and V from mouse and keyboard
                mainCamera.updateFromMouse();
                
                //~ glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);

                // Bind and Set our "myTextureSampler" sampler to use Texture
                //~ texture->set(TextureID);
                //~ scene->skyboxShader()->use();
                //~ glm::mat4 projection = mainCamera.projectionMatrix();
            //~ glm::mat4 view = mainCamera.viewMatrix();
                //~ glUniformMatrix4fv(scene->skyboxShader()->getUniformLocation("projection"), 
                    //~ 1, GL_FALSE, &projection[0][0]);
                //~ glUniformMatrix4fv(scene->skyboxShader()->getUniformLocation("view"), 
                    //~ 1, GL_FALSE, &view[0][0]);
                    
                
                int newState = glfwGetKey( window.internal(), GLFW_KEY_K );
                if (newState == GLFW_RELEASE && oldState == GLFW_PRESS) {
                       scene->light()->type((Light::Type)(((int)scene->light()->type() + 1) % (int)Light::PhongWithNormal));
                    oldState = newState;
                }
            
                
                scene->render();

                // Swap buffers
                window.swap();
                glfwPollEvents();

                // Swap buffers
                window.postDrawingEvent();
                //~ DEBUG(Debug::Info, "\rFPS: %f", 1.f / (glfwGetTime() - time_last_frame));
                time_last_frame = glfwGetTime();
            }

            // Check if the ESC key was pressed or the window was closed
            while( window.keyStatus(GLFW_KEY_ESCAPE) != GLFW_PRESS &&
                    !window.shouldClose());

            DEBUG(Debug::Info, "\n");
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
    
    //~ } catch (ShaderException* e){
            //~ std::cout << "Shader exception: " << e->what() << std::endl;
            //~ glfwTerminate();
            //~ return EXIT_FAILURE;
    
    }
    glfwTerminate();
    
    return EXIT_SUCCESS;
}
