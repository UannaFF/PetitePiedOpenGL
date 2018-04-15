#include "light.hpp"
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

#include <vector>
#include <map>
#include <set>
#include <string>
#include <iostream>
#include <iomanip>

#include "common.hpp"
#include "material.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include "scene.hpp"

using namespace std;
using namespace glm;


Light::Light():
	_pos(0,0,0),
	_color(1,1,1)
{

    _shader = Shader::fromFiles( "shaders/vertexshader_light.glsl", "shaders/fragment_light.glsl");
    //Scene *sc = Scene::import("./res/sphere.dae", _shader);
    //_mesh = (Mesh*)sc->rootNode()->children().begin()->second;
}

void Light::draw(glm::mat4 proj, glm::mat4 view){
	/*glm::mat4 mat = glm::translate(glm::mat4(1.f), _pos);
    _shader->use();
    _shader->setVec3("color", _color);
    //_mesh->bind();
    //_shader->setMat4("model", mat);
    _mesh->draw(proj ,view ,mat);
    _shader->deuse();*/
}
