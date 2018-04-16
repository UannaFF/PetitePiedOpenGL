#ifndef ANIMATIONS_H
#define ANIMATIONS_H

#include <vector>
#include <map>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <assimp/quaternion.h>

#include <iostream>

#include "models.hpp"
#include "texture.hpp"
#include "common.hpp"

class KeyFrame;
class Animation;
class NodeAnimator;
class Node;
class Bone;
class Key;
class Scene;

class Key {
    public:
        virtual glm::mat4 value() const = 0;
};

class PositionKey : public Key {
    public:
        PositionKey(glm::vec3 value): _value(value)
        {}
        inline glm::mat4 value() const { return glm::translate(glm::mat4(0), _value); }
        inline glm::vec3 position() const { return _value; }
    private:
        glm::vec3 _value;
};

class RotationKey : public Key {
    public:
        RotationKey(glm::quat value);
        inline glm::mat4 value() const { return glm::toMat4(_value); }
        inline glm::quat rotation() const { return _value; }
    private:
        glm::quat _value;
};

class Channel {
    public:
        Channel(Node* n, std::vector<Bone*> b = std::vector<Bone*>()): _node(n), _bones(b), _positions_keys(), _rotations_keys() {}
        
        void addKey(float t, PositionKey* k) {
            _positions_keys.insert(std::make_pair(t, k));                
        }
        void addKey(float t, RotationKey* k) {
            _rotations_keys.insert(std::make_pair(t, k));                
        }
        
        void applyBones(float AnimationTime, glm::mat4& currentTransformation, const glm::mat4& GlobalInverseTransform);
        
        std::pair<std::pair<float, PositionKey*>, std::pair<float, PositionKey*>> getPosKeys(float AnimationTime) const;
        std::pair<std::pair<float, RotationKey*>, std::pair<float, RotationKey*>> getRotKeys(float AnimationTime) const;
    
        inline Node* node() const { return _node; }
    private:
        std::map<float, PositionKey*> _positions_keys;
        std::map<float, RotationKey*> _rotations_keys;
        std::vector<Bone*> _bones;
        Node* _node;
};

class Animation {
    public:
        Animation(std::string name, double duration, double tick_per_sec): 
            _name(name),
            _duration(duration),
            _tick_per_sec(tick_per_sec) {}
            
        void addChannel(Channel* c) { _channel.insert(std::make_pair(c->node(), c)); } 
        
        void applyBones(float AnimationTime, Scene* s);
        
        inline float tickPerSec() const { return _tick_per_sec != 0. ? _tick_per_sec : 25.0f; }
        inline float timeInTick(float TimeInSeconds) const { return TimeInSeconds * _tick_per_sec; }
        inline float duration() const { return _duration; }
        inline std::string name() const { return _name; }
    private:
        void _recusive_bones(Node* n, Scene* s, const glm::mat4& globalInverseTransform, const glm::mat4& ParentTransform, float AnimationTime);
        
        std::string _name;
        std::map<Node*, Channel*> _channel;
        //~ std::vector<Channel*> _channel;
        double _tick_per_sec;
        double _duration;
};

#endif
