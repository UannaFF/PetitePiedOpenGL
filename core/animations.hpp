#ifndef ANIMATIONS_H
#define ANIMATIONS_H

#include <vector>
#include <map>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/quaternion.h>

#include <iostream>

#include "models.hpp"
#include "texture.hpp"
#include "common.hpp"

class Joint;
class KeyFrame;
class Animation;
class Quaternion;
class NodeAnimator;
class Node;
class Bone;
class Key;
class Scene;



/**
 * A quaternion simply represents a 3D rotation. The maths behind it is quite
 * complex (literally; it involves complex numbers) so I wont go into it in too
 * much detail. The important things to note are that it represents a 3d
 * rotation, it's very easy to interpolate between two quaternion rotations
 * (which would not be easy to do correctly with Euler rotations or rotation
 * matrices), and you can convert to and from matrices fairly easily. So when we
 * need to interpolate between rotations we'll represent them as quaternions,
 * but when we need to apply the rotations to anything we'll convert back to a
 * matrix.
 * 
 * @author Karl
 *
 */
class Quaternion {

	private:
        float _x, _y, _z, _w;

    public:
        /**
         * Creates a quaternion and normalizes it.
         * 
         * @param x
         * @param y
         * @param z
         * @param w
         */
        Quaternion(float x, float y, float z, float w);
        Quaternion();
        
        inline float x() const { return _x; }
        inline float y() const { return _y; }
        inline float z() const { return _z; }
        inline float w() const { return _w; }
        
        inline void x(float v) { _x = v; }
        inline void y(float v) { _y = v; }
        inline void z(float v) { _z = v; }
        inline void w(float v) { _w = v; }

        /**
         * Normalizes the quaternion.
         */
        void normalize();

        /**
         * Converts the quaternion to a 4x4 matrix representing the exact same
         * rotation as this quaternion. (The rotation is only contained in the
         * top-left 3x3 part, but a 4x4 matrix is returned here for convenience
         * seeing as it will be multiplied with other 4x4 matrices).
         * 
         * More detailed explanation here:
         * http://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToMatrix/
         * 
         * @return The rotation matrix which represents the exact same rotation as
         *         this quaternion.
         */
        glm::mat4 toRotationMatrix() const;

        /**
         * Extracts the rotation part of a transformation matrix and converts it to
         * a quaternion using the magic of maths.
         * 
         * More detailed explanation here:
         * http://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/index.htm
         * 
         * @param matrix
         *            - the transformation matrix containing the rotation which this
         *            quaternion shall represent.
         */
        static Quaternion fromMatrix(glm::mat4& matrix);

        /**
         * Interpolates between two quaternion rotations and returns the resulting
         * quaternion rotation. The interpolation method here is "nlerp", or
         * "normalized-lerp". Another mnethod that could be used is "slerp", and you
         * can see a comparison of the methods here:
         * https://keithmaggio.wordpress.com/2011/02/15/math-magician-lerp-slerp-and-nlerp/
         * 
         * and here:
         * http://number-none.com/product/Understanding%20Slerp,%20Then%20Not%20Using%20It/
         * 
         * @param a
         * @param b
         * @param blend
         *            - a value between 0 and 1 indicating how far to interpolate
         *            between the two quaternions.
         * @return The resulting interpolated rotation in quaternion format.
         */
        Quaternion interpolate(Quaternion b, float blend);
        static Quaternion fromAi(aiQuaternion q);

};

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
        RotationKey(Quaternion value): _value(value) {}
        inline glm::mat4 value() const { return _value.toRotationMatrix(); }
        inline Quaternion rotation() const { return _value; }
    private:
        Quaternion _value;
};

class Channel {
    public:
        Channel(Node* n, std::vector<Bone*> b): _node(n), _bones(b) {}
        
        void addKey(float t, Key* k) {
            auto it = _keys.find(t);
            if (it != _keys.end()){
                it->second.push_back(k);
            } else {
                std::vector<Key*> v;
                v.push_back(k);
                _keys.insert(std::pair<float, std::vector<Key*>>(t, v));
            }
                
        }
        
        void applyBones(float AnimationTime, glm::mat4& currentTransformation, const glm::mat4& GlobalInverseTransform);
        
        std::map<float, std::vector<Key*>>::const_iterator getPosKeys(float AnimationTime) const {
            for (auto it = _keys.begin(); it != _keys.end();)
                if ((++it)->first > AnimationTime)
                    return --it;
            return _keys.begin();
        }
    
        inline Node* node() const { return _node; }
    private:
        Node* _node;
        std::map<float, std::vector<Key*>> _keys;
        std::vector<Bone*> _bones;
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
        double _tick_per_sec;
        double _duration;
};

#endif
