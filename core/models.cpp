#include "models.hpp"
#include "common.hpp"
#include "texture.hpp"
#include "shader.hpp"
#include "material.hpp"
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags


#include <iostream>
#include <assert.h>
#include <string.h>

int Bone::LAST_ID = 0;

void Bone::dumpToBuffer(std::vector<int>& vertex_buff, std::vector<float>& weight_buff){
    DEBUG(Debug::Info, "Bone has %d record to dump\n", _weights.size());
    std::cout << _offset;

    normalize();
    
    int i = 0;
    
    for (std::pair<uint, float> p: _weights){
        //~ DEBUG(Debug::Info, " - BoneID: %d VID: %d, weight: %f\n", _boneid, p.first, p.second);
        
        int offset = 0;
        for (; weight_buff[p.first * 4 + offset] != 0.0; offset++){}
        
        if (offset >= 4)
            continue;
            
        assert(offset < 4);
        
        vertex_buff[p.first * 4 + offset] = _boneid;
        weight_buff[p.first * 4 + offset] = p.second;
    }
}

void Bone::normalize() {
    float mag = 0;
    
    for (std::pair<uint, float> p: _weights)
        mag += p.second * p.second;
        
    mag = sqrt(mag);
        
    for (std::pair<uint, float> p: _weights)
        _weights[p.first] = p.second / mag;
}

void Bone::setTransformation(const glm::mat4& mat) {
    DEBUG(Debug::Info, "-- Update bone id %d at %d\n", _boneid, _frag);
    glUniformMatrix4fv(_frag, 1, GL_TRUE, &(mat * _offset)[0][0]);
    //~ glUniformMatrix4fv(_frag, 1, GL_TRUE, &(mat)[0][0]);
    std::cout << (mat * _offset);
}

Mesh::Mesh():
    _len_points(0),
    _mode(GL_TRIANGLES)
{    
    glGenVertexArrays(1, &_vertex_array_id);
                
    glGenBuffers(1, &_vertexbuffer);
    glGenBuffers(1, &_uvbuffer);
    glGenBuffers(1, &_normal);
    glGenBuffers(1, &_indice);
    glGenBuffers(1, &_bones_id);
    glGenBuffers(1, &_weight);

    _material = nullptr;
}

void Mesh::setVertex(std::vector<GLfloat> vertex)
{            
    glBindVertexArray(_vertex_array_id);
    DEBUG(Debug::Info, "Mesh has %d vertices\n", vertex.size());
    glBindBuffer(GL_ARRAY_BUFFER, _vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, vertex.size() * sizeof(GLfloat), &vertex[0], GL_STATIC_DRAW);
    _len_points = vertex.size();

}

void Mesh::setUV(std::vector<GLfloat> uv)
{    
    glBindVertexArray(_vertex_array_id);
    glBindBuffer(GL_ARRAY_BUFFER, _uvbuffer);
    glBufferData(GL_ARRAY_BUFFER, uv.size() * sizeof(GLfloat), &uv[0], GL_STATIC_DRAW);
}

void Mesh::setNormal(std::vector<GLfloat> normal)
{    
    glBindVertexArray(_vertex_array_id);
    DEBUG(Debug::Info, "Mesh has %d normal\n", normal.size());
    glBindBuffer(GL_ARRAY_BUFFER, _normal);
    glBufferData(GL_ARRAY_BUFFER, normal.size() * sizeof(GLfloat), &normal[0], GL_STATIC_DRAW);
}

void Mesh::setIndice(std::vector<unsigned short> indices)
{    
    glBindVertexArray(_vertex_array_id);
    glBindBuffer(GL_ARRAY_BUFFER, _indice);
    glBufferData(GL_ARRAY_BUFFER, indices.size() * sizeof(GLfloat), &indices[0], GL_STATIC_DRAW);
}

Mesh::~Mesh()
{        
    glDeleteBuffers(1, &_vertexbuffer);
    glDeleteBuffers(1, &_uvbuffer);
    glDeleteBuffers(1, &_normal);
    glDeleteBuffers(1, &_indice);
    glDeleteBuffers(1, &_bones_id);
    glDeleteBuffers(1, &_weight);
    glDeleteVertexArrays(1, &_vertex_array_id);
}

void Mesh::draw(Shader* usedShader){
    if (_material)
        _material->apply(usedShader);
    
    // 1rst attribute buffer : vertices
    glEnableVertexAttribArray(GL_LAYOUT_VERTEXARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, _vertexbuffer);
    glVertexAttribPointer(
        GL_LAYOUT_VERTEXARRAY,                  // attribute. No particular reason for 0, but must match the layout in the shader.
        3,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        0,                  // stride
        (void*)0            // array buffer offset
    );

    // 2nd attribute buffer : UVs
    if(_uvbuffer) {
        glEnableVertexAttribArray(GL_LAYOUT_UV);
        glBindBuffer(GL_ARRAY_BUFFER, _uvbuffer);
        glVertexAttribPointer(GL_LAYOUT_UV, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    }
    
    // 3rd attribute buffer : Normals
    if(_normal) {
        glEnableVertexAttribArray(GL_LAYOUT_NORMAL);
        glBindBuffer(GL_ARRAY_BUFFER, _normal);
        glVertexAttribPointer(GL_LAYOUT_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    }

    // 4th attribute buffer : Bones
    if(_bones_id) {
        glEnableVertexAttribArray(GL_LAYOUT_BONES);
        glBindBuffer(GL_ARRAY_BUFFER, _bones_id);
        glVertexAttribPointer(GL_LAYOUT_BONES, 4, GL_INT, GL_FALSE, 0, nullptr);
    }

    // 5th attribute buffer : Weight
    if(_weight) {
        glEnableVertexAttribArray(GL_LAYOUT_WEIGHT);
        glBindBuffer(GL_ARRAY_BUFFER, _weight);
        glVertexAttribPointer(GL_LAYOUT_WEIGHT, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
    }

    // Index buffer
    if(_indice)
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indice);
    
    // Draw !
    glDrawArrays(_mode, 0, _len_points);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(3);
    glDisableVertexAttribArray(4);
}

void Mesh::setBones(std::vector<Bone*> bones, Shader* s)
{   
    std::vector<GLint> bones_buffer(_len_points * 4, 0);
    std::vector<GLfloat> weight_buffer(_len_points * 4, 0.0);
    
    s->use();
    int bone_id = 0;
    for (Bone* b: bones){        
        b->id(bone_id);
        b->dumpToBuffer(bones_buffer, weight_buffer);
        
        // Binding to the GLSL bones
        DEBUG(Debug::Info, "gBones[%d]\n", bone_id);
        b->frag_id(s->getUniformLocation("gBones[" + std::to_string(bone_id) +"]"));
        bone_id++;
        b->setTransformation(glm::mat4(1.f));
    }        
    for (int v = 0; v < _len_points; v ++){
        float total_weight = 0; 
        for (int w = 0; w < 4; w++)
            total_weight += weight_buffer[v * 4 + w];
        //~ if (total_weight != 1.f)
            printf("w of %d:%f\n", v, total_weight);
    }
    
    // Uploading to GPU
   	glBindBuffer(GL_ARRAY_BUFFER, _bones_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLint) * bones_buffer.size(), &bones_buffer[0], GL_STATIC_DRAW);
   	glBindBuffer(GL_ARRAY_BUFFER, _weight);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * weight_buffer.size(), &weight_buffer[0], GL_STATIC_DRAW);

}

Skybox::Skybox() {



}

void Skybox::setEverything() {
        //Define a big cube
    std::vector<GLfloat> points = {
      -10.0f,  10.0f, -10.0f, //back cube assume top left back corner
      -10.0f, -10.0f, -10.0f,
       10.0f, -10.0f, -10.0f,
       10.0f, -10.0f, -10.0f,
       10.0f,  10.0f, -10.0f,
      -10.0f,  10.0f, -10.0f,
      
      -10.0f, -10.0f,  10.0f, //left square front, left, bottom corner
      -10.0f, -10.0f, -10.0f,
      -10.0f,  10.0f, -10.0f,
      -10.0f,  10.0f, -10.0f,
      -10.0f,  10.0f,  10.0f,
      -10.0f, -10.0f,  10.0f,
      
       10.0f, -10.0f, -10.0f, //right square
       10.0f, -10.0f,  10.0f,
       10.0f,  10.0f,  10.0f,
       10.0f,  10.0f,  10.0f,
       10.0f,  10.0f, -10.0f,
       10.0f, -10.0f, -10.0f,
       
      -10.0f, -10.0f,  10.0f, //front square
      -10.0f,  10.0f,  10.0f,
       10.0f,  10.0f,  10.0f,
       10.0f,  10.0f,  10.0f,
       10.0f, -10.0f,  10.0f,
      -10.0f, -10.0f,  10.0f,
      
      -10.0f,  10.0f, -10.0f, //top square
       10.0f,  10.0f, -10.0f,
       10.0f,  10.0f,  10.0f,
       10.0f,  10.0f,  10.0f,
      -10.0f,  10.0f,  10.0f,
      -10.0f,  10.0f, -10.0f,
      
      -10.0f, -10.0f, -10.0f, //bottom square
      -10.0f, -10.0f,  10.0f,
       10.0f, -10.0f, -10.0f,
       10.0f, -10.0f, -10.0f,
      -10.0f, -10.0f,  10.0f,
       10.0f, -10.0f,  10.0f
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

    setVertex(points);
    setIndice(indices);
    setUV(uv);
}

Skybox::~Skybox() {

}
