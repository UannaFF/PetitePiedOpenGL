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
                    //
                }
            }
            if (pos && rot)
                break;
        }
    }
    
    glm::mat4 trans(_node->inverseTransformation());
    if (!pos)
        pos = new glm::vec3(0.f);
    if (!rot)
        rot = new glm::quat(glm::vec3(0., 0., 0.));
        
    glm::mat4 translate(1.f);
    translate[0][3] = pos->x;
    translate[1][3] = pos->y;
    translate[2][3] = pos->z;
        
    _node->setTransformation(glm::inverse(trans) * (glm::translate(glm::toMat4(*rot), *pos)) * trans);
    std::cout << "Time: " << AnimationTime << ", trans:" << _node->transformation() << std::endl;

    delete pos;
    delete rot;
    //~ for (Bone*b: _bones){
        //~ b->node()->setTransformation(currentTransformation);
    //~ }
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

