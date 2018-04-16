#include "animations.hpp"

#include <math.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "models.hpp"
#include "scene.hpp"
#include "common.hpp"


void Channel::applyBones(float AnimationTime, glm::mat4& currentTransformation, const glm::mat4& GlobalInverseTransform)
{                
    float DeltaTime, Factor;
    
    std::pair<std::pair<float, PositionKey*>, std::pair<float, PositionKey*>> poskey = getPosKeys(AnimationTime);
    
    glm::vec3 pos(0.f);
    if (poskey.first.second != nullptr && poskey.second.second != nullptr){
        DeltaTime = poskey.second.first - poskey.first.first;
        Factor = (AnimationTime - poskey.first.first) / DeltaTime;
        pos = glm::vec3(poskey.first.second->position());
        pos += Factor * (poskey.second.second->position() - poskey.first.second->position());
    }
    
    std::pair<std::pair<float, ScaleKey*>, std::pair<float, ScaleKey*>> scakey = getScaKeys(AnimationTime);
    
    glm::vec3 sca(1.f);
    if (scakey.first.second != nullptr && scakey.second.second != nullptr){
        DeltaTime = scakey.second.first - scakey.first.first;
        Factor = (AnimationTime - scakey.first.first) / DeltaTime;
        sca = glm::vec3(scakey.first.second->dimenssion());
        sca += Factor * (scakey.second.second->dimenssion() - scakey.first.second->dimenssion());
    }
    
    std::pair<std::pair<float, RotationKey*>, std::pair<float, RotationKey*>> rotkey = getRotKeys(AnimationTime);
    glm::quat rot(0.f, 0.f, 0.f, 0.f);
    if (rotkey.first.second != nullptr && rotkey.second.second != nullptr){
        DeltaTime = rotkey.second.first - rotkey.first.first;
        Factor = (AnimationTime - rotkey.first.first) / DeltaTime;
        rot = glm::mix(rotkey.first.second->rotation(), rotkey.second.second->rotation(), Factor);
    }
    
    glm::mat4 trans(_node->inverseTransformation());
        
    glm::mat4 translate(1.f);
    translate[0][3] = pos.x;
    translate[1][3] = pos.y;
    translate[2][3] = pos.z;
        
    _node->setTransformation(glm::inverse(trans) * (glm::toMat4(rot) * glm::scale(translate, sca)) * trans);

    for (Bone*b: _bones){
        b->node()->setTransformation(currentTransformation);
    }
}
std::pair<std::pair<float, PositionKey*>, std::pair<float, PositionKey*>> Channel::getPosKeys(float AnimationTime) const {
    if (!_positions_keys.empty()){
        auto next = _positions_keys.begin();
        auto prev = next++;
        for (; next != _positions_keys.end();){
            if (next->first > AnimationTime)
                return std::make_pair(*prev, *next);
            else
                prev = next++;
        }
    }
    return std::make_pair(std::make_pair(0, nullptr), std::make_pair(0, nullptr));
}
std::pair<std::pair<float, RotationKey*>, std::pair<float, RotationKey*>> Channel::getRotKeys(float AnimationTime) const {
    if (!_rotations_keys.empty()){
        auto next = _rotations_keys.begin();
        auto prev = next++;
        for (; next != _rotations_keys.end();){
            if (next->first > AnimationTime)
                return std::make_pair(*prev, *next);
            else
                prev = next++;
        }
    }
    return std::make_pair(std::make_pair(0, nullptr), std::make_pair(0, nullptr));
}
std::pair<std::pair<float, ScaleKey*>, std::pair<float, ScaleKey*>> Channel::getScaKeys(float AnimationTime) const {
    if (!_scales_keys.empty()){
        auto next = _scales_keys.begin();
        auto prev = next++;
        for (; next != _scales_keys.end();){
            if (next->first > AnimationTime)
                return std::make_pair(*prev, *next);
            else
                prev = next++;
        }
    }
    return std::make_pair(std::make_pair(0, nullptr), std::make_pair(0, nullptr));
}

RotationKey::RotationKey(glm::quat value): _value(value){
    _value = normalize(value);
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
            _recusive_bones((Node*)child.second, s, localTransform, globalInverseTransform,AnimationTime);
}

