#include "world.hpp"
#include "../core/scene.hpp"
#include "../core/shader.hpp"
#include "../core/particlesVolcano.h"
#include "../core/light.hpp"
#include "../core/animations.hpp"

#include <glm/gtc/matrix_transform.hpp>


Scene* DinoWorld::buildScene(){
    Shader* shader = Shader::fromFiles( "shaders/vertexshader_material.glsl", "shaders/fragment_material.glsl");         
    Scene* main_scene = Scene::import("object/volcano_lowpoly.dae", shader);
    
    Scene* veg = Scene::import("object/végétation.dae", shader);
    Scene* diplo_scene = Scene::import("object/diplo.dae", shader);
    Scene* boat_scene = Scene::import("object/boat.dae", shader);
    Scene* ptero_scene = Scene::import("object/ptero.dae", shader);
    
    main_scene->rootNode()->addChild("vegetation", veg->rootNode());
    
    Node* pmNode = new Node("pmnode", glm::rotate(glm::scale(glm::mat4(1.f), glm::vec3(0.06f)), glm::radians(180.f), glm::vec3(1.f, 0., 0.))* translation(-2.6, -0.5, -0.4), main_scene, main_scene->rootNode());
    
    Node * diplo1 = new Node(*diplo_scene->rootNode()->find("diplodocus"));
    Node * diplo2 = new Node(*diplo_scene->rootNode()->find("diplodocus"));
    
    glm::mat4 diplo_rotate_scale = glm::scale(
        glm::rotate(diplo1->transformation(), glm::radians(-90.f), glm::vec3(1.0f, 0.0f, 0.0f)),
        glm::vec3(0.1f)
    );
    
    diplo1->setTransformation(diplo_rotate_scale);
    diplo_rotate_scale = glm::rotate(diplo_rotate_scale, glm::radians(90.f), glm::vec3(0.0f, 0.0f, -1.0f));
    diplo2->setTransformation(diplo_rotate_scale * translation(1.5, -2, 0.05));

    diplo1->parent(main_scene->rootNode());
    diplo2->parent(main_scene->rootNode()); 
    pmNode->parent(main_scene->rootNode());   
    
    pmNode->addChild("particlesspawner", new ParticuleManager(Shader::fromFiles( "shaders/vertexshader_particule.glsl", "shaders/fragment_particule.glsl")));
    main_scene->rootNode()->addChild("pmnode", pmNode);
    main_scene->rootNode()->addChild("diplo", diplo1);
    main_scene->rootNode()->addChild("diplo2", diplo2);
    
    ptero_scene->findNode("Plane")->parent(main_scene->rootNode());
    main_scene->rootNode()->addChild("diplo2", ptero_scene->findNode("Plane"));
    ptero_scene->findNode("Plane")->setTransformation(glm::scale(glm::mat4(1.f), glm::vec3(0.2f)) * translation(-3, 0, 6));

    glm::mat4 boat_rotate_scale = glm::rotate(glm::rotate(glm::scale(boat_scene->rootNode()->transformation(), glm::vec3(0.1f)), glm::radians(-90.f), glm::vec3(1.0f, 0.0f, 0.0f)), glm::radians(90.f), glm::vec3(0.0f, 0.0f, 1.0f));
    
    boat_scene->rootNode()->find("boat")->setTransformation(boat_rotate_scale * translation(4, -0.3, 0.05));
    boat_scene->rootNode()->find("boat")->parent(main_scene->rootNode());
    main_scene->rootNode()->addChild("boat", boat_scene->rootNode()->find("boat"));
    
    Scene* arm_scene = Scene::import("object/armclean.dae", main_scene->defaultShader());
    arm_scene->rootNode()->setTransformation(glm::mat4(1.f));
    arm_scene->rootNode()->find("Base_low_001")->setTransformation(glm::mat4(1.f));
    
    arm_scene->rootNode()->find("Arm_low_001")->parent(arm_scene->rootNode()->find("Base_low_001"));
    arm_scene->rootNode()->find("Arm_low_001")->setTransformation(glm::mat4(1.f));
    
    arm_scene->rootNode()->find("Arm2_Low_001")->parent(arm_scene->rootNode()->find("Arm_low_001"));
    arm_scene->rootNode()->find("Arm2_Low_001")->setTransformation(glm::mat4(1.f) * translation(0, 0, 0.12));
    
    arm_scene->rootNode()->find("FingerL_low_001")->parent(arm_scene->rootNode()->find("Arm2_Low_001"));
    arm_scene->rootNode()->find("FingerL_low_001")->setTransformation(glm::mat4(1.f) * translation(-0.10, -0.01, 0));
    
    arm_scene->rootNode()->find("FingerR_low_001")->parent(arm_scene->rootNode()->find("Arm2_Low_001"));
    arm_scene->rootNode()->find("FingerR_low_001")->setTransformation(glm::mat4(1.f) * translation(-0.10, 0.01, 0));
    
    arm_scene->rootNode()->find("Rail_low_001")->parent(main_scene->rootNode()->find("boat"));
    arm_scene->rootNode()->find("Rail_low_001")->setTransformation(glm::rotate(translation(-1.6, -3.45, -0.05), glm::radians(-90.f), glm::vec3(0.0f, 0.0f, 1.0f)));
    main_scene->rootNode()->find("boat")->addChild("rail", arm_scene->rootNode()->find("Rail_low_001"));
    
    // Arm animation
    Animation* opening_hand = new Animation("FingerClose", 80, 20);
    Channel* c = new Channel(main_scene->rootNode()->find("FingerL_low_001"));
    c->addKey(0, new RotationKey(glm::quat(glm::vec3(0, glm::radians(0.), 0))));
    c->addKey(40, new RotationKey(glm::quat(glm::vec3(0, glm::radians(60.), 0))));
    c->addKey(80, new RotationKey(glm::quat(glm::vec3(0, glm::radians(0.), 0))));
    c->addKey(0, new PositionKey(glm::vec3(-0.90, 0.01, 0)));
    c->addKey(80, new PositionKey(glm::vec3(-0.90, 0.01, 0)));
    opening_hand->addChannel(c);
    c = new Channel(main_scene->rootNode()->find("FingerR_low_001"));
    c->addKey(0, new RotationKey(glm::quat(glm::vec3(0, glm::radians(0.), 0))));
    c->addKey(40, new RotationKey(glm::quat(glm::vec3(0, glm::radians(-60.), 0))));
    c->addKey(80, new RotationKey(glm::quat(glm::vec3(0, glm::radians(0.), 0))));
    c->addKey(0, new PositionKey(glm::vec3(-0.90, -0.01, 0)));
    c->addKey(80, new PositionKey(glm::vec3(-0.90, -0.01, 0)));
    opening_hand->addChannel(c);
    
    main_scene->addAnimation(opening_hand);  
    
    // Ptero animation  
    Animation* ptero_flying = new Animation("PteroAnim", 400, 20);
    c = new Channel(ptero_scene->findNode("Plane"));
    c->addKey(0, new PositionKey(glm::vec3(-20, -20, 150.)));
    c->addKey(380, new PositionKey(glm::vec3(-20, -20, -150.)));
    c->addKey(390, new PositionKey(glm::vec3(150, -20, -150.)));
    c->addKey(400, new PositionKey(glm::vec3(150, -20, 150.)));
    c->addKey(0, new ScaleKey(glm::vec3(0.2)));
    c->addKey(400, new ScaleKey(glm::vec3(0.2)));
    ptero_flying->addChannel(c);
    
    main_scene->addAnimation(ptero_flying); 
    
    // Adding light    
    Light* light = new Light();
    light->setPos( glm::vec3(-3.61, -4, 0.23));
    light->setColor(glm::vec3(1.0, 1.0, 1.0));    
    main_scene->addLight(light);
    
    return main_scene;
}

glm::mat4 translation(float x, float y, float z){
    glm::mat4 trans(1.f);
    trans[0][3] = x;
    trans[1][3] = y;
    trans[2][3] = z;
    return trans;
}
