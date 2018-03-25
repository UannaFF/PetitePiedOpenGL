#include "models.hpp"
#include "common.hpp"


VertexArray::VertexArray():
    _len_points(0), _mode(GL_TRIANGLES)
{    
    glGenVertexArrays(1, &_vertex_array_id);
                
    glGenBuffers(1, &_vertexbuffer);
    glGenBuffers(1, &_uvbuffer);
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

VertexArray::~VertexArray()
{        
    glDeleteBuffers(1, &_vertexbuffer);
    glDeleteBuffers(1, &_uvbuffer);
    glDeleteVertexArrays(1, &_vertex_array_id);
}

void VertexArray::draw(){
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

    // Draw the triangle !
    glDrawArrays(_mode, 0, _len_points); // 12*3 indices starting at 0 -> 12 triangles
    //~ DEBUG(Debug::Info, "Drawing!\n");

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}
