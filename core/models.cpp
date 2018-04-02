#include "models.hpp"
#include "common.hpp"
#include "texture.hpp"
#include "shader.hpp"
#include "material.hpp"
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

#define GL_LAYOUT_VERTEXARRAY 0
#define GL_LAYOUT_UV 1
#define GL_LAYOUT_NORMAL 2
//~ #define GL_LAYOUT_INDICE 0
#define GL_LAYOUT_BONES 3
#define GL_LAYOUT_WEIGHT 4

#include <iostream>
#include <assert.h>
#include <string.h>

int Bone::LAST_ID = 0;

void Bone::dumpToBuffer(int* vertex_buff, float* weight_buff){
    DEBUG(Debug::Info, "Bone has %d record to dump\n", _weights.size());
    
    int i = 0;
    
    for (std::pair<uint, std::vector<float>> p: _weights){
        DEBUG(Debug::Info, " - BoneID: %d VID: %d, weight: %f\n", _boneid, p.first, p.second[0]);
        //~ assert(p.second.size() == 4);
        vertex_buff[p.first] = _boneid;
        weight_buff[p.first] = p.second[0];
        //~ memcpy(weight_buff + i, &(p.second[0]), p.second.size() * sizeof(float));
        //~ memset(weight_buff + (i++) + p.second.size() * sizeof(float), 0, (4 - p.second.size()) * sizeof(float));
        //~ i++;
    }
}
void Bone::setTransformation(const glm::mat4& mat) {
    //~ DEBUG(Debug::Info, "-- Update bone at %d\n", _frag);
    //~ glUniformMatrix4fv(_frag, 1, GL_FALSE, &(mat * _offset)[0][0]);
    glUniformMatrix4fv(_frag, 1, GL_FALSE, &(mat)[0][0]);
    //~ std::cout << (mat * _offset);
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
}

void Mesh::setVertex(std::vector<GLfloat> vertex)
{            
    DEBUG(Debug::Info, "Mesh has %d vertices\n", vertex.size());
    glBindBuffer(GL_ARRAY_BUFFER, _vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, vertex.size() * sizeof(GLfloat), &vertex[0], GL_STATIC_DRAW);
    _len_points = vertex.size();
}

void Mesh::setUV(std::vector<GLfloat> uv)
{    
    glBindBuffer(GL_ARRAY_BUFFER, _uvbuffer);
    glBufferData(GL_ARRAY_BUFFER, uv.size() * sizeof(GLfloat), &uv[0], GL_STATIC_DRAW);
}

void Mesh::setNormal(std::vector<GLfloat> normal)
{    
    DEBUG(Debug::Info, "Mesh has %d normal\n", normal.size());
    glBindBuffer(GL_ARRAY_BUFFER, _normal);
    glBufferData(GL_ARRAY_BUFFER, normal.size() * sizeof(GLfloat), &normal[0], GL_STATIC_DRAW);
}

void Mesh::setIndice(std::vector<unsigned short> indices)
{    
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
    glEnableVertexAttribArray(GL_LAYOUT_UV);
    glBindBuffer(GL_ARRAY_BUFFER, _uvbuffer);
    glVertexAttribPointer(GL_LAYOUT_UV, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    
    // 3rd attribute buffer : Normals
    glEnableVertexAttribArray(GL_LAYOUT_NORMAL);
    glBindBuffer(GL_ARRAY_BUFFER, _normal);
    glVertexAttribPointer(GL_LAYOUT_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    // 4th attribute buffer : Bones
    glEnableVertexAttribArray(GL_LAYOUT_BONES);
    glBindBuffer(GL_ARRAY_BUFFER, _bones_id);
    glVertexAttribPointer(GL_LAYOUT_BONES, 1, GL_INT, GL_FALSE, 0, nullptr);

    // 5th attribute buffer : Weight
    glEnableVertexAttribArray(GL_LAYOUT_WEIGHT);
    glBindBuffer(GL_ARRAY_BUFFER, _weight);
    glVertexAttribPointer(GL_LAYOUT_WEIGHT, 1, GL_FLOAT, GL_FALSE, 0, nullptr);

    // Index buffer
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
    //~ int num_of_bone_data = 0;
    
    GLint* bones_buffer = new GLint[_len_points];
    GLfloat* weight_buffer = new GLfloat[_len_points];
    
    s->use();
    for (Bone* b: bones){
        //~ num_of_bone_data += b->size();
        
        // Dumping raw data to be passed to GLSL
        b->id(Bone::LAST_ID);
        b->dumpToBuffer(bones_buffer, weight_buffer);
        // Binding to the GLSL bones
        b->frag_id(s->getUniformLocation("gBones[" + std::to_string(Bone::LAST_ID++) +"]"));
        b->setTransformation(glm::mat4(1.f));
    }
    
    // Uploading to GPU
   	glBindBuffer(GL_ARRAY_BUFFER, _bones_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLint) * _len_points, bones_buffer, GL_STATIC_DRAW);
   	glBindBuffer(GL_ARRAY_BUFFER, _weight);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * _len_points, weight_buffer, GL_STATIC_DRAW);

    // Cleaning
    delete [] bones_buffer;
    delete [] weight_buffer;
}
