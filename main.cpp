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
#include "core/particlesVolcano.h"

#include "assets/utils.hpp"

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
            Shader* shader = Shader::fromFiles( "shaders/vertexshader_material.glsl", "shaders/fragment_material.glsl");
            Scene* scene = Scene::import("res/Dinosaure/Environement/Sol+Eau/volcano_lowpoly.dae", shader);
            
            Scene* veg = Scene::import("res/Dinosaure/Environement/Végétation/végétation.dae", shader);
            scene->rootNode()->addChild("vegetation", veg->rootNode());
            
            Node* pmNode = new Node("pmnode", glm::rotate(glm::scale(glm::mat4(1.f), glm::vec3(0.06f)), glm::radians(180.f), glm::vec3(1.f, 0., 0.)), scene, scene->rootNode());
            pmNode->addChild("vegetation", new ParticuleManager(Shader::fromFiles( "shaders/vertexshader_particule.glsl", "shaders/fragment_particule.glsl")));
            scene->rootNode()->addChild("pmnode", pmNode);
            
            Scene* diplo_scene = Scene::import("res/Dinosaure/diplodocus/diplo.dae", shader);
            Node * diplo1 = new Node(*diplo_scene->rootNode());
            Node * diplo2 = new Node(*diplo_scene->rootNode());
            
            glm::mat4 diplo_rotate_scale = glm::scale(
                glm::rotate(diplo1->transformation(), glm::radians(180.f), glm::vec3(1.0f, 0.0f, 0.0f)),
                glm::vec3(0.1f)
            );
            glm::mat4 trans(1.f);
            trans[0][3] = 2.54998;
            trans[1][3] = -2.2;
            trans[2][3] = 0.05;
            std::cout << trans;
            diplo1->setTransformation(diplo_rotate_scale);
            diplo_rotate_scale = glm::rotate(diplo_rotate_scale, glm::radians(90.f), glm::vec3(0.0f, 0.0f, -1.0f));
            diplo2->setTransformation(diplo_rotate_scale * trans);

            scene->rootNode()->addChild("diplo", diplo1);
            scene->rootNode()->addChild("diplo", diplo2);
            
            //~ Scene* boat_scene = Scene::import("res/Dinosaure/Boat/boat.dae", shader);
            //~ glm::mat4 boat_rotate_scale = 
                //~ glm::rotate(glm::scale(glm::inverse(glm::translate(boat_scene->rootNode()->transformation(), glm::vec3(0.1,0,0.f))),
                //~ glm::vec3(0.1f)
            //~ ), glm::radians(-90.f), glm::vec3(1.0f, 0.0f, 0.0f));
            
            //~ std::cout << boat_rotate_scale;
            
            //~ boat_scene->rootNode()->setTransformation(boat_rotate_scale);
            //~ scene->rootNode()->addChild("boat", boat_scene->rootNode());
            
            //~ Scene* scene = new Scene;
            //~ scene->setShader(Shader::fromFiles( "shaders/vertexshader_marker.glsl", "shaders/fragment_marker.glsl"), Scene::BonesDebugShader);
            //~ scene->setShader(Shader::fromFiles( "shaders/vertexshader_material.glsl", "shaders/fragment_material.glsl"));
            //~ scene->setRootNode(new Node("main", glm::mat4(1.f), scene));
            //~ scene->displayNodeTree();
            
            // Cube test
            //~ VertexArray* cube = new VertexArray;
            //~ cube->setVertex({1,  1,  -1,
                             //~ 1, -1,  -1,
                             //~ -1, -1, -1,
                             //~ -1,  1, -1,
                             //~ 1,  1,  1,
                             //~ 1, -1,  1,
                             //~ -1, -1, 1,
                             //~ -1, 1,  1});
            //~ cube->setIndice({0, 2, 3,  
                             //~ 7, 5, 4,  
                             //~ 4, 1, 0,  
                             //~ 5, 2, 1,  
                             //~ 2, 7, 3,  
                             //~ 0, 7, 4,  
                             //~ 0, 1, 2,  
                             //~ 7, 6, 5,  
                             //~ 4, 5, 1,  
                             //~ 5, 6, 2,  
                             //~ 2, 6, 7,  
                             //~ 0, 3, 7});
            //~ scene->rootNode()->addChild("cube", new Mesh(Shader::fromFiles( "shaders/vertexshader_marker.glsl", "shaders/fragment_marker.glsl"), cube, std::vector<Bone*>()));

            
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
            scene->rootNode()->addChild("marker", new ReferenceMarker(Shader::fromFiles( "shaders/vertexshader_marker.glsl", "shaders/fragment_marker.glsl")));
            
            // Skybox
            scene->setSkybox("skybox_sky", "shaders/Skyboxshadingvertex.glsl","shaders/Skyboxshadingfragment.glsl" );
            
            
            
            window.hideCursor();
            window.centerCursor();
            
            scene->playAnimation(0);
            glm::vec3 lightPos = glm::vec3(0,-2.7, 4);   
            
            float time_last_frame = glfwGetTime();
            DEBUG(Debug::Info, "\n");
            
            do{ 
                // Clear the screen
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                
                scene->defaultShader()->use();
                
                if (glfwGetKey( window.internal(), GLFW_KEY_X ) == GLFW_PRESS)
                    trans[0][3] += 0.05;
                if (glfwGetKey( window.internal(), GLFW_KEY_W ) == GLFW_PRESS)
                    trans[0][3] -= 0.05;
                if (glfwGetKey( window.internal(), GLFW_KEY_Q ) == GLFW_PRESS)
                    trans[1][3] += 0.05;
                if (glfwGetKey( window.internal(), GLFW_KEY_D ) == GLFW_PRESS)
                    trans[1][3] -= 0.05;
                if (glfwGetKey( window.internal(), GLFW_KEY_U ) == GLFW_PRESS)
                    trans[2][3] += 0.05;
                if (glfwGetKey( window.internal(), GLFW_KEY_P ) == GLFW_PRESS)
                    trans[2][3] -= 0.05;
                    
                std::cout << trans[0][3] <<", " << trans[1][3] << ", " << trans[2][3] << std::endl;
                diplo2->setTransformation(diplo_rotate_scale * trans);
                scene->defaultShader()->setVec3("light.position", lightPos);
                    
                //~ glm::vec3 lightColor;
                //~ lightColor.x = sin(glfwGetTime() * 2.0f);
                //~ lightColor.y = sin(glfwGetTime() * 0.7f);
                //~ lightColor.z = sin(glfwGetTime() * 1.3f);
                //~ glm::vec3 diffuseColor = lightColor   * glm::vec3(0.5f); // decrease the influence
                //~ glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f); // low influence
                scene->defaultShader()->setVec3("light.ambient", 1.0f, 1.0f, 1.0f);
                scene->defaultShader()->setVec3("light.diffuse", 1.0f, 1.0f, 1.0f);
                scene->defaultShader()->setVec3("light.specular", 1.0f, 1.0f, 1.0f);
                
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
    
    } catch (ShaderException* e){
            std::cout << "Shader exception: " << e->what() << std::endl;
            glfwTerminate();
            return EXIT_FAILURE;
    
    }
    glfwTerminate();
    
    return EXIT_SUCCESS;
}
