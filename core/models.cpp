#include "models.hpp"
#include "common.hpp"
#include "texture.hpp"
#include "shader.hpp"
#include "material.hpp"
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

Mesh::Mesh():
    _len_points(0),
    _mode(GL_TRIANGLES),
    _rootJoint(nullptr),
    _jointCount(0),
    _animator(nullptr)
{    
    glGenMeshs(1, &_vertex_array_id);
                
    glGenBuffers(1, &_vertexbuffer);
    glGenBuffers(1, &_uvbuffer);
    glGenBuffers(1, &_normal);
    glGenBuffers(1, &_indice);
    glGenBuffers(1, &_bones_id);
}

void Mesh::setVertex(std::vector<GLfloat> vertex)
{            
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
    glDeleteVertexArrays(1, &_vertex_array_id);
}

void Mesh::draw(Shader* usedShader){

    if (_rootJoint){
        if (_animator)
            _animator->update();
        
        m_mesh.BoneTransform(RunningTime, Transforms);

        for (uint i = 0 ; i < Transforms.size() ; i++) {
            m_pEffect->SetBoneTransform(i, Transforms[i]);
        } 
    }
    
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

void Mesh::setBones(std::vector<GLfloat> bones)
{                
   	glBindBuffer(GL_ARRAY_BUFFER, _bones_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VertexBoneData) * bones.size(), &bones[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(3);
    glVertexAttribIPointer(3, 4, GL_INT, sizeof(VertexBoneData), (const GLvoid*)0);
    glEnableVertexAttribArray(4);    
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(VertexBoneData), (const GLvoid*)16);
}

std::vector<glm::mat4> Mesh::getJointTransforms() {
    std::vector<glm::mat4> jointMatrices;
    jointMatrices.reserve(_jointCount);
    addJointsToArray(_rootJoint, jointMatrices);
    return jointMatrices;
}

void Mesh::addJointsToArray(Joint* headJoint, std::vector<glm::mat4>& jointMatrices) {
    jointMatrices[headJoint->index()] = headJoint->getAnimatedTransform();
    for (Joint* childJoint : headJoint->children()) {
        addJointsToArray(childJoint, jointMatrices);
    }
}

glm::mat4 Mesh::boneTransform(std::vector<glm::mat4>& transforms)
{
    glm::mat4 identity(1.f);
    
    float current_time = (float)((double)GetCurrentTimeMillis() - mAnimation->startTime()) / 1000.0f;
    float AnimationTime = fmod(mAnimation->ticksTime(), mAnimation->duration());

    ReadNodeHeirarchy(AnimationTime, m_pScene->mRootNode, Identity);

    transforms.resize(m_NumBones);

    for (uint i = 0 ; i < m_NumBones ; i++) {
        transforms[i] = m_BoneInfo[i].FinalTransformation;
    }
} 

void Mesh::calcInterpolatedRotation(Quaternion& Out, float animationTime, const aiNodeAnim* pNodeAnim)
{
    // we need at least two values to interpolate...
    if (pNodeAnim->mNumRotationKeys == 1) {
        Out = pNodeAnim->mRotationKeys[0].mValue;
        return;
    }

    uint RotationIndex = FindRotation(AnimationTime, pNodeAnim);
    uint NextRotationIndex = (RotationIndex + 1);
    assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);
    float DeltaTime = pNodeAnim->mRotationKeys[NextRotationIndex].mTime - pNodeAnim->mRotationKeys[RotationIndex].mTime;
    float Factor = (AnimationTime - (float)pNodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;
    assert(Factor >= 0.0f && Factor <= 1.0f);
    const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
    const aiQuaternion& EndRotationQ = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;
    aiQuaternion::Interpolate(Out, StartRotationQ, EndRotationQ, Factor);
    Out = Out.Normalize();
} 

void Mesh::setRootBone(Joint* j){
    _rootBone = j;
    _bonesCount = j->size();
}
