#include "models.hpp"
#include "common.hpp"
#include "scene.hpp"
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
GLint Mesh::VA_PRIMITIVE = GL_TRIANGLES;

void Bone::dumpToBuffer(std::vector<int>& vertex_buff, std::vector<float>& weight_buff){
    DEBUG(Debug::Info, "Bone has %d record to dump\n", _weights.size());
    std::cout << _offset;
    
    int i = 0;
    
    for (std::pair<uint, float> p: _weights){
        
        int offset = 0;
        for (; weight_buff[p.first * 4 + offset] != 0.0; offset++){}
        
        if (offset >= 4)
            continue;
            
        assert(offset < 4);
        
        vertex_buff[p.first * 4 + offset] = _boneid;
        weight_buff[p.first * 4 + offset] = p.second;
    }
}
glm::mat4 Bone::transformation() const {
    //~ DEBUG(Debug::Info, "-- Update bone id %d from node %s\n", _boneid, _node->name().c_str());
    //~ return _node->world_transformation() * _offset;
    return _node->world_transformation();
}

VertexArray::VertexArray():
    _len_points(0), _indice(0)
{    
    glGenVertexArrays(1, &_vertex_array_id);
                
    glGenBuffers(1, &_vertexbuffer);
    glGenBuffers(1, &_uvbuffer);
    glGenBuffers(1, &_normal);
    glGenBuffers(1, &_bones_id);
    glGenBuffers(1, &_weight);
}

void VertexArray::setVertex(std::vector<GLfloat> vertex)
{            
    glBindVertexArray(_vertex_array_id);
    DEBUG(Debug::Info, "VertexArray has %d vertices\n", vertex.size() / 3);
    
    glEnableVertexAttribArray(GL_LAYOUT_VERTEXARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, _vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, vertex.size() * sizeof(GLfloat), &vertex[0], GL_STATIC_DRAW);
    glVertexAttribPointer(GL_LAYOUT_VERTEXARRAY, 3,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        0,                  // stride
        (void*)0            // array buffer offset
    );
    _len_points = vertex.size() / 3;
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

}

void VertexArray::setUV(std::vector<GLfloat> uv)
{    
    glBindVertexArray(_vertex_array_id);
    DEBUG(Debug::Info, "VertexArray has %d UV\n", uv.size());
    
    glEnableVertexAttribArray(GL_LAYOUT_UV);
    glBindBuffer(GL_ARRAY_BUFFER, _uvbuffer);
    glBufferData(GL_ARRAY_BUFFER, uv.size() * sizeof(GLfloat), &uv[0], GL_STATIC_DRAW);
    glVertexAttribPointer(GL_LAYOUT_UV, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void VertexArray::setNormal(std::vector<GLfloat> normal)
{    
    glBindVertexArray(_vertex_array_id);
    DEBUG(Debug::Info, "VertexArray has %d normal\n", normal.size() / 3);
    
    glEnableVertexAttribArray(GL_LAYOUT_NORMAL);
    glBindBuffer(GL_ARRAY_BUFFER, _normal);
    glBufferData(GL_ARRAY_BUFFER, normal.size() * sizeof(GLfloat), &normal[0], GL_STATIC_DRAW);
    glVertexAttribPointer(GL_LAYOUT_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void VertexArray::setIndice(std::vector<unsigned short> indices)
{    
    glBindVertexArray(_vertex_array_id);   
    glGenBuffers(1, &_indice); 
    DEBUG(Debug::Info, "VertexArray has %d faces\n", indices.size() / 3);
       
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indice);      
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLushort), &indices[0], GL_STATIC_DRAW);  
            
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); 
    glBindVertexArray(0);          
}

VertexArray::~VertexArray()
{        

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(3);
    glDisableVertexAttribArray(4);
    glDeleteBuffers(1, &_vertexbuffer);
    glDeleteBuffers(1, &_uvbuffer);
    glDeleteBuffers(1, &_normal);
    glDeleteBuffers(1, &_bones_id);
    glDeleteBuffers(1, &_weight);
    
    if (_indice)
        glDeleteBuffers(1, &_indice);
    
    glDeleteVertexArrays(1, &_vertex_array_id);
}

void VertexArray::draw(GLint primitive){
    glBindVertexArray(_vertex_array_id);
    
    if (_indice){
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indice);     
        int size;  glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
        glDrawElements(primitive, size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); 
    } else
        glDrawArrays(primitive, 0, _len_points);
        
    glBindVertexArray(0);
}

void VertexArray::setBones(std::vector<Bone*> bones, Shader* s)
{   
    std::vector<GLint> bones_buffer(_len_points * 4, 0);
    std::vector<GLfloat> weight_buffer(_len_points * 4, 0.0);
    
    s->use();
    int bone_id = 0;
    for (Bone* b: bones){        
        b->id(bone_id++);
        b->dumpToBuffer(bones_buffer, weight_buffer);
    }        
    //~ for (int v = 0; v < _len_points; v ++){
        //~ float total_weight = 0; 
            //~ total_weight += weight_buffer[v * 4 + w];
        //~ if (total_weight != 1.f)
            //~ printf("w of %d:%f\n", v, total_weight);
    //~ }
    //~ for (int b = 0; b < 4 * _len_points; b++)
        //~ DEBUG(Debug::Info, " - BoneID: %d VID: %d, weight: %f, offset: %d\n", bones_buffer[b], b/4, weight_buffer[b], b % 4);
    
    // Uploading to GPU    
    glBindVertexArray(_vertex_array_id);
    DEBUG(Debug::Info, "VertexArray has %d bone info\n", _len_points);
    
    glEnableVertexAttribArray(GL_LAYOUT_BONES);
    glBindBuffer(GL_ARRAY_BUFFER, _bones_id);
	glBufferData(GL_ARRAY_BUFFER, bones_buffer.size() * sizeof(GLint), &bones_buffer[0], GL_STATIC_DRAW);
    glVertexAttribPointer(GL_LAYOUT_BONES, 4, GL_INT, GL_FALSE, 0, nullptr);
    
    
    glEnableVertexAttribArray(GL_LAYOUT_WEIGHT);
    glBindBuffer(GL_ARRAY_BUFFER, _weight);
	glBufferData(GL_ARRAY_BUFFER, weight_buffer.size() * sizeof(GLfloat), &weight_buffer[0], GL_STATIC_DRAW);
    glVertexAttribPointer(GL_LAYOUT_WEIGHT, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);   
    glBindVertexArray(0);
}

void Mesh::draw(glm::mat4 projection, glm::mat4 view, glm::mat4 model){
    _shader->use();    

    // setup camera geometry parameters
    _shader->setMat4("projection", projection);
    _shader->setMat4("view", view);
    _shader->setMat4("model", model);
    
    // bone world transform matrices need to be passed for skinning
    for (Bone* b: _bones){
        _shader->setMat4("gBones[" + std::to_string(b->id()) + "]", b->transformation(), GL_TRUE);
        std::cout << "gBones[" + std::to_string(b->id()) + "]" << std::endl << b->transformation();
    }

    if (_material)
        _material->apply(_shader);
        
    // draw mesh vertex array
    _vao->draw(GL_LINE_STRIP);

    // leave with clean OpenGL state, to make it easier to detect problems
    _shader->deuse();
}

Mesh::~Mesh(){
    for (Bone* b: _bones)
        delete b;
    delete _vao;
}

Skybox::Skybox(Shader* s):
    Mesh::Mesh(s, nullptr, std::vector<Bone*>())
{
    VertexArray* va = new VertexArray();

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

    va->setVertex(points);
    va->setIndice(indices);
    va->setUV(uv);
    
    setVAO(va);
}
