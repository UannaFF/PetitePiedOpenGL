#include "utils.hpp"
#include "../core/shader.hpp"
#include <vector>

ReferenceMarker::ReferenceMarker(Shader* s):
    Mesh(s, new VertexArray, std::vector<Bone*>())
{
    std::vector<float> pos_color({0, 0, 0,
                                  3, 0, 0,
                                  0, 0, 0,
                                  0, 3, 0,
                                  0, 0, 0,
                                  0, 0, 3});
                                  
    VAO()->setVertex(pos_color);  
}
                
void ReferenceMarker::draw(glm::mat4 projection, glm::mat4 view, glm::mat4 model){
    _shader->use();    

    // setup camera geometry parameters
    _shader->setMat4("projection", projection);
    _shader->setMat4("view", view);
    _shader->setMat4("model", model, GL_TRUE);
    
    // draw mesh vertex array
    VAO()->draw(GL_LINES);

    // leave with clean OpenGL state, to make it easier to detect problems
    _shader->deuse();
}
