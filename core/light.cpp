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
    _mesh = new Mesh(_shader, nullptr, std::vector<Bone*>());
    
    VertexArray* va = new VertexArray();

    std::vector<GLfloat> points = {
      -0.1f,  0.1f, -0.1f, //back cube assume top left back corner
      -0.1f, -0.1f, -0.1f,
       0.1f, -0.1f, -0.1f,
       0.1f, -0.1f, -0.1f,
       0.1f,  0.1f, -0.1f,
      -0.1f,  0.1f, -0.1f,
      
      -0.1f, -0.1f,  0.1f, //left square front, left, bottom corner
      -0.1f, -0.1f, -0.1f,
      -0.1f,  0.1f, -0.1f,
      -0.1f,  0.1f, -0.1f,
      -0.1f,  0.1f,  0.1f,
      -0.1f, -0.1f,  0.1f,
      
       0.1f, -0.1f, -0.1f, //right square
       0.1f, -0.1f,  0.1f,
       0.1f,  0.1f,  0.1f,
       0.1f,  0.1f,  0.1f,
       0.1f,  0.1f, -0.1f,
       0.1f, -0.1f, -0.1f,
       
      -0.1f, -0.1f,  0.1f, //front square
      -0.1f,  0.1f,  0.1f,
       0.1f,  0.1f,  0.1f,
       0.1f,  0.1f,  0.1f,
       0.1f, -0.1f,  0.1f,
      -0.1f, -0.1f,  0.1f,
      
      -0.1f,  0.1f, -0.1f, //top square
       0.1f,  0.1f, -0.1f,
       0.1f,  0.1f,  0.1f,
       0.1f,  0.1f,  0.1f,
      -0.1f,  0.1f,  0.1f,
      -0.1f,  0.1f, -0.1f,
      
      -0.1f, -0.1f, -0.1f, //bottom square
      -0.1f, -0.1f,  0.1f,
       0.1f, -0.1f, -0.1f,
       0.1f, -0.1f, -0.1f,
      -0.1f, -0.1f,  0.1f,
       0.1f, -0.1f,  0.1f
    };

    std::vector<unsigned short> indices = {
        0, 1, 2,
        3, 4, 5,
        6, 7, 8,
        9, 10, 11,
        12, 13, 14,
        15, 16, 17,
        18, 19, 20,
        21, 22, 23,
        24, 25, 26,
        27, 28, 29,
        30, 31, 32,
        33, 34, 35,
    };

    std::vector<GLfloat> uv = {
        0, 1, //-1,
        0, 0, //-1,
        1, 0, //-1,
        1, 0, //-1,
        1, 1, //-1,
        0, 1, //-1,

        0, 1, //1,
        0, 0, //-1,
        1, 0, //-1,
        1, 0, //-1,
        1, 1, //1,
        0, 1, //1,

        0, 1, //-1,
        0, 0, //1,
        1, 0, //1,
        1, 0, //1,
        1, 1, //-1,
        0, 1, //-1,

        0, 1, //1,
        0, 0, //1,
        1, 0, //1,
        1, 0, //1,
        1, 1, //1,
        0, 1, //1,

        0, 1, //-1,
        0, 0, //-1,
        1, 0, //1,
        1, 0,// 1,
        1, 1, //1,
        0, 1, //-1,

        0, 1, //-1,
        0, 0, //1,
        1, 0, //-1,
        1, 0, //-1,
        1, 1, //1,
        0, 1, //1,

    };

    std::vector<GLfloat> normals = {
        
    };

    va->setVertex(points);
    va->setIndice(indices);
    va->setUV(uv);
    
    _mesh->setVAO(va);
}

void Light::draw(glm::mat4 proj, glm::mat4 view){
    _shader->use();
	glm::mat4 mat = glm::mat4(6.f);//glm::translate(, _pos);
    _shader->setVec3("color", _color.x, _color.y, _color.z);
    _shader->deuse();
    //_mesh->bind();
    //_shader->setMat4("model", mat);
    _mesh->draw(proj ,view ,mat);
    
}
