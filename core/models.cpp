#include "models.hpp"
#include "common.hpp"
#include "texture.hpp"
#include "shader.hpp"
#include "material.hpp"
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

VertexArray::VertexArray():
    _len_points(0), _mode(GL_TRIANGLES)
{    
    glGenVertexArrays(1, &_vertex_array_id);
                
    glGenBuffers(1, &_vertexbuffer);
    glGenBuffers(1, &_uvbuffer);
    glGenBuffers(1, &_normal);
    glGenBuffers(1, &_indice);
}

void VertexArray::setVertex(std::vector<GLfloat> vertex)
{            
    glBindBuffer(GL_ARRAY_BUFFER, _vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, vertex.size() * sizeof(GLfloat), &vertex[0], GL_STATIC_DRAW);
    _len_points = vertex.size();
}

void VertexArray::setUV(std::vector<GLfloat> uv)
{    
    glBindBuffer(GL_ARRAY_BUFFER, _uvbuffer);
    glBufferData(GL_ARRAY_BUFFER, uv.size() * sizeof(GLfloat), &uv[0], GL_STATIC_DRAW);
}

void VertexArray::setNormal(std::vector<GLfloat> normal)
{    
    glBindBuffer(GL_ARRAY_BUFFER, _normal);
    glBufferData(GL_ARRAY_BUFFER, normal.size() * sizeof(GLfloat), &normal[0], GL_STATIC_DRAW);
}

void VertexArray::setIndice(std::vector<unsigned short> indices)
{    
    glBindBuffer(GL_ARRAY_BUFFER, _indice);
    glBufferData(GL_ARRAY_BUFFER, indices.size() * sizeof(GLfloat), &indices[0], GL_STATIC_DRAW);
}

VertexArray::~VertexArray()
{        
    glDeleteBuffers(1, &_vertexbuffer);
    glDeleteBuffers(1, &_uvbuffer);
    glDeleteBuffers(1, &_normal);
    glDeleteBuffers(1, &_indice);
    glDeleteVertexArrays(1, &_vertex_array_id);
}

void VertexArray::draw(Shader* usedShader){
    if (_material)
        _material->apply(usedShader);
    
    // 1rst attribute buffer : vertices
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, _vertexbuffer);
    glVertexAttribPointer(
        0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
        3,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        0,                  // stride
        (void*)0            // array buffer offset
    );

    // 2nd attribute buffer : UVs
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, _uvbuffer);
    glVertexAttribPointer(
        1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
        2,                                // size : U+V => 2
        GL_FLOAT,                         // type
        GL_FALSE,                         // normalized?
        0,                                // stride
        (void*)0                          // array buffer offset
    );
    
    // 3rd attribute buffer : Normals
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, _normal);
    glVertexAttribPointer(
        2,                                // attribute
        3,                                // size
        GL_FLOAT,                         // type
        GL_FALSE,                         // normalized?
        0,                                // stride
        (void*)0                          // array buffer offset
    );

    // Index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indice);
    
    // Draw !
    glDrawArrays(_mode, 0, _len_points);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}

