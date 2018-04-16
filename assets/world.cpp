#include "world.hpp"
#include "../core/scene.hpp"
#include "../core/shader.hpp"
#include "../core/particlesVolcano.h"

#include <glm/gtc/matrix_transform.hpp>


Scene* DinoWorld::buildScene(){
    Shader* shader = Shader::fromFiles( "shaders/vertexshader_material.glsl", "shaders/fragment_material.glsl");            
    Scene* main_scene = Scene::import("res/Dinosaure/Environement/Sol+Eau/volcano_lowpoly.dae", shader);
            
    Scene* veg = Scene::import("res/Dinosaure/Environement/Végétation/végétation.dae", shader);
    main_scene->rootNode()->addChild("vegetation", veg->rootNode());
    
    Node* pmNode = new Node("pmnode", glm::rotate(glm::scale(glm::mat4(1.f), glm::vec3(0.06f)), glm::radians(180.f), glm::vec3(1.f, 0., 0.))* translation(-2.9, -0.2, -0.2), main_scene, main_scene->rootNode());
    pmNode->addChild("vegetation", new ParticuleManager(Shader::fromFiles( "shaders/vertexshader_particule.glsl", "shaders/fragment_particule.glsl")));
    //~ pmNode->setTransformation(glm::mat4(1.f) * translation(-1, -0.3, -0.2));
    main_scene->rootNode()->addChild("pmnode", pmNode);
    
    Scene* diplo_scene = Scene::import("res/Dinosaure/diplodocus/diplo.dae", shader);
    Node * diplo1 = new Node(*diplo_scene->rootNode());
    Node * diplo2 = new Node(*diplo_scene->rootNode());
    
    glm::mat4 diplo_rotate_scale = glm::scale(
        glm::rotate(diplo1->transformation(), glm::radians(180.f), glm::vec3(1.0f, 0.0f, 0.0f)),
        glm::vec3(0.1f)
    );
    
    diplo1->setTransformation(diplo_rotate_scale);
    diplo_rotate_scale = glm::rotate(diplo_rotate_scale, glm::radians(90.f), glm::vec3(0.0f, 0.0f, -1.0f));
    diplo2->setTransformation(diplo_rotate_scale * translation(1.5, -2, 0.05));

    main_scene->rootNode()->addChild("diplo", diplo1);
    main_scene->rootNode()->addChild("diplo2", diplo2);
    
    Scene* boat_scene = Scene::import("res/Dinosaure/Boat/boat.dae", shader);
    Scene* arm_scene = Scene::import("res/arm/brasmécanique.dae", shader);
    arm_scene->rootNode()->setTransformation(glm::mat4(1.f));
    boat_scene->rootNode()->addChild("rail", arm_scene->rootNode());
    
    glm::mat4 boat_rotate_scale = glm::rotate(glm::rotate(glm::scale(boat_scene->rootNode()->transformation(), glm::vec3(0.1f)), glm::radians(-90.f), glm::vec3(1.0f, 0.0f, 0.0f)), glm::radians(90.f), glm::vec3(0.0f, 0.0f, 1.0f));
    
    boat_scene->rootNode()->setTransformation(boat_rotate_scale * translation(6, -0.3, -0.45));
    main_scene->rootNode()->addChild("boat", boat_scene->rootNode());
    
    return main_scene;
}

glm::mat4 translation(float x, float y, float z){
    glm::mat4 trans(1.f);
    trans[0][3] = x;
    trans[1][3] = y;
    trans[2][3] = z;
    return trans;
}
