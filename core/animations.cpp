#include "animations.hpp"

#include <math.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "models.hpp"
#include "scene.hpp"
#include "common.hpp"


void Channel::applyBones(float AnimationTime, glm::mat4& currentTransformation, const glm::mat4& GlobalInverseTransform)
{                
    float beginTime[2] = {-1., -1};
    
    glm::vec3 beginpos, * pos = nullptr;
    glm::quat beginrot, * rot = nullptr;
    
    for (std::map<float, std::vector<Key*>>::const_iterator key = getPosKeys(AnimationTime); key != _keys.end(); key++){
        for (Key* k: key->second){
            if (!pos && dynamic_cast<PositionKey*>(k)){
                if (beginTime[0] < 0.){
                    beginTime[0] = key->first;
                    beginpos = dynamic_cast<PositionKey*>(k)->position();
                }
                else {           
                    PositionKey& end = *dynamic_cast<PositionKey*>(k);
                    float DeltaTime = key->first - beginTime[0];
                    float Factor = (AnimationTime - beginTime[0]) / DeltaTime;
                    pos = new glm::vec3(beginpos);
                    (*pos) += Factor * (end.position() - beginpos);
                }
            }
            else if (!rot && dynamic_cast<RotationKey*>(k)){
                if (beginTime[1] < 0.){
                    beginTime[1] = key->first;
                    beginrot = dynamic_cast<RotationKey*>(k)->rotation();
                }
                else {           
                    RotationKey& end = *dynamic_cast<RotationKey*>(k);
                    float DeltaTime = key->first - beginTime[1];
                    float Factor = (AnimationTime - beginTime[1]) / DeltaTime;
                    rot = new glm::quat(glm::mix(beginrot, end.rotation(), Factor));
                }
            }
            if (pos && rot)
                break;
        }
    }
    
    //~ std::cout << "Time: " << AnimationTime << ", position:" << pos << ", rot:" << rot << std::endl;
    //~ currentTransformation *= (glm::translate(glm::mat4(1.f), *pos) * rot->toRotationMatrix());
    //~ std::cout << "global:" << std::endl << GlobalInverseTransform <<  std::endl << "local:" << std::endl << currentTransformation << std::endl;

    glm::mat4 trans(glm::mat4(1.f));
    trans[0][3] = pos->x;
    trans[1][3] = pos->y;
    trans[2][3] = pos->z;
    
    _node->setTransformation(trans * glm::toMat4(*rot));
    //~ _node->setTransformation(trans);
    std::cout << "Time: " << AnimationTime << ", trans:" << _node->transformation() << std::endl;

    delete pos;
    delete rot;
    //~ for (Bone*b: _bones){
        //~ b->node()->setTransformation(currentTransformation);
    //~ }
}

//~ Quaternion::Quaternion():
    //~ Quaternion(0, 0, 0, 0)
//~ {}
    
//~ Quaternion::Quaternion(float x, float y, float z, float w):
    //~ _x(x),
    //~ _y(y),
    //~ _z(z),
    //~ _w(w)
//~ {
    //~ normalize();
//~ }

//~ Quaternion Quaternion::fromAi(aiQuaternion q){
    //~ return Quaternion(q.x, q.y, q.z, q.w);
//~ }

//~ void Quaternion::normalize() {
    //~ float mag = sqrt(_w * _w + _x * _x + _y * _y + _z * _z);
    //~ _w /= mag;
    //~ _x /= mag;
    //~ _y /= mag;
    //~ _z /= mag;
//~ }

//~ glm::mat4 Quaternion::toRotationMatrix() const {
    //~ glm::mat4 matrix(0);
    //~ float xy = _x * _y, xz = _x * _z, xw = _x * _w, yz = _y * _z, yw = _y * _w, zw = _z * _w;
    //~ float xSquared = _x * _x, ySquared = _y * _y, zSquared = _z * _z;
    //~ matrix[0][0] = 1 - 2 * (ySquared + zSquared);
    //~ matrix[0][1] = 2 * (xy - zw);
    //~ matrix[0][2] = 2 * (xz + yw);

    //~ matrix[1][0] = 2 * (xy + zw);
    //~ matrix[1][1] = 1 - 2 * (xSquared + zSquared);
    //~ matrix[1][2] = 2 * (yz - xw);

    //~ matrix[2][0] = 2 * (xz - yw);
    //~ matrix[2][1] = 2 * (yz + xw);
    //~ matrix[2][2] = 1 - 2 * (xSquared + ySquared);

    //~ matrix[3][3] = 1;
    //~ return matrix;
//~ }

//~ Quaternion Quaternion::fromMatrix(glm::mat4& matrix) {
    //~ float w, x, y, z;
    //~ float diagonal = matrix[0][0] + matrix[1][1] + matrix[2][2];
    //~ if (diagonal > 0) {
        //~ float w4 = sqrt(diagonal + 1) * 2;
        //~ w = w4 / 4;
        //~ x = (matrix[2][1] - matrix[1][2]) / w4;
        //~ y = (matrix[0][2] - matrix[2][0]) / w4;
        //~ z = (matrix[1][0] - matrix[0][1]) / w4;
    //~ } else if ((matrix[0][0] > matrix[1][1]) && (matrix[0][0] > matrix[2][2])) {
        //~ float x4 = sqrt(1 + matrix[0][0] - matrix[1][1] - matrix[2][2]) * 2;
        //~ w = (matrix[2][1] - matrix[1][2]) / x4;
        //~ x = x4 / 4;
        //~ y = (matrix[0][1] + matrix[1][0]) / x4;
        //~ z = (matrix[0][2] + matrix[2][0]) / x4;
    //~ } else if (matrix[1][1] > matrix[2][2]) {
        //~ float y4 = sqrt(1 + matrix[1][1] - matrix[0][0] - matrix[2][2]) * 2;
        //~ w = (matrix[0][2] - matrix[2][0]) / y4;
        //~ x = (matrix[0][1] + matrix[1][0]) / y4;
        //~ y = y4 / 4;
        //~ z = (matrix[1][2] + matrix[2][1]) / y4;
    //~ } else {
        //~ float z4 = sqrt(1 + matrix[2][2] - matrix[0][0] - matrix[1][1]) * 2;
        //~ w = (matrix[1][0] - matrix[0][1]) / z4;
        //~ x = (matrix[0][2] + matrix[2][0]) / z4;
        //~ y = (matrix[1][2] + matrix[2][1]) / z4;
        //~ z = z4 / 4;
    //~ }
    //~ return Quaternion(x, y, z, w);
//~ }

//~ Quaternion Quaternion::interpolate(Quaternion b, float blend) {
    //~ Quaternion result(0, 0, 0, 1);
    //~ float dot = w() * b.w() + x() * b.x() + y() * b.y() + z() * b.z();
    //~ float blendI = 1 - blend;
    //~ if (dot < 0) {
        //~ result.w(blendI * w() + blend * -b.w());
        //~ result.x(blendI * x() + blend * -b.x());
        //~ result.y(blendI * y() + blend * -b.y());
        //~ result.z(blendI * z() + blend * -b.z());
    //~ } else {
        //~ result.w(blendI * w() + blend * b.w());
        //~ result.x(blendI * x() + blend * b.x());
        //~ result.y(blendI * y() + blend * b.y());
        //~ result.z(blendI * z() + blend * b.z());
    //~ }
    //~ result.normalize();
    //~ return result;
//~ }

RotationKey::RotationKey(glm::quat value): _value(value){
}

void Animation::applyBones(float AnimationTime, Scene* s)
{    
    glm::mat4 ParentTransform(1.f);
    _recusive_bones(s->rootNode(), s, glm::inverse(s->rootNode()->transformation()), ParentTransform, AnimationTime);
}

void Animation::_recusive_bones(Node* n, Scene* s, const glm::mat4& globalInverseTransform, const glm::mat4& ParentTransform, float AnimationTime){
    glm::mat4 localTransform = ParentTransform;
    if (_channel.find(n) != _channel.end()){
        Channel* c = _channel.find(n)->second;
        c->applyBones(AnimationTime, localTransform, globalInverseTransform);
    
    }
        
    for (std::pair<std::string, Drawable*> child: n->children())
        if (dynamic_cast<Node*>(child.second))
            _recusive_bones((Node*)child.second, s, globalInverseTransform, localTransform, AnimationTime);
}

