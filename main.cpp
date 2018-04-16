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
#include "core/animations.hpp"

#include "assets/utils.hpp"
#include "assets/world.hpp"

using namespace glm;
static int oldState = GLFW_RELEASE;

int main(int argc, char** argv){

    bool show_fps = false, disable_skybox = false, free_camera = false, display_tree = false;
    char* marker_attach = NULL;
    
	// Initialise GLFW
	if( !glfwInit() )
	{
	    std::cerr << "Failed to initialize GLFW\n";
	    return -1;
	}
    
    int argCount;
    for (argc--, argv++; argc > 0; argc -= argCount, argv += argCount){
        argCount = 1;
        if (!strcmp (*argv, "--attach-marker")){
            argCount++;
            if (argc > 1)
                marker_attach = *(argv + 1);
            else
                DEBUG(Debug::Error, "--attach-marker requires a positionnal argument.\n");
        } else if (!strcmp (*argv, "--show-fps")){
            show_fps = true;
        } else if (!strcmp (*argv, "--display-tree")){
            display_tree = true;
        } else if (!strcmp (*argv, "--disable-skybox")){
            disable_skybox = true;
        } else if (!strcmp (*argv, "--free-camera")){
            free_camera = true;
        } else {
            fprintf(stderr, "petit_pied [--attach-marker <node_name> | --free-camera | --show-fps | --display-tree | --disable-skybox]\n\n");
        }
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

            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LESS); 

            Scene* scene = DinoWorld::buildScene();   
             
            TextureLoader* tl = new TextureLoader;
            tl->loadTextures(scene->rootNode());       
            
            //~ Camera mainCamera;
            scene->setCamera(&mainCamera);

            // Marker
            if (marker_attach){
                if (scene->findNode(marker_attach)){
                    Node* markerNode = new Node("MarkerNode", glm::mat4(1.f), scene, scene->rootNode());
                    markerNode->addChild("markerMesh", new ReferenceMarker(Shader::fromFiles( "shaders/vertexshader_marker.glsl", "shaders/fragment_marker.glsl")));
                    markerNode->parent(scene->findNode(marker_attach));
                    scene->findNode(marker_attach)->addChild("marker", markerNode);
                } else
                    DEBUG(Debug::Error, "Cannot find the node '%s' to attach marker on. Skipping\n", marker_attach);
            }
            
            // Skybox
            if (!disable_skybox)
                scene->setSkybox("skybox_sky", "shaders/vertexshader_skybox.glsl","shaders/fragment_skybox.glsl" );
            

            window.hideCursor();
            window.centerCursor();
            
            scene->playAnimation(0);
            
            if (display_tree)
                scene->displayNodeTree();  
            
            float time_last_frame = glfwGetTime();
            DEBUG(Debug::Info, "\n");
            
            
            do{ 
                // Clear the screen
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                
                // Update P and V from mouse and keyboard
                mainCamera.updateFromMouse();
                
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
                
                if (show_fps)
                    DEBUG(Debug::Info, "\rFPS: %f", 1.f / (glfwGetTime() - time_last_frame));
                    
                time_last_frame = glfwGetTime();
            }

            // Check if the ESC key was pressed or the window was closed
            while( window.keyStatus(GLFW_KEY_ESCAPE) != GLFW_PRESS &&
                    !window.shouldClose());

            DEBUG(Debug::Info, "\n");
            // Cleanup VBO and shader
            //~ delete shader;
            //~ delete texture;
            
    //~ } catch (OpenGLException* e){
            //~ std::cout << "OpenGL exception: " << e->what() << std::endl;
            //~ glfwTerminate();
            //~ return EXIT_FAILURE;
    
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
