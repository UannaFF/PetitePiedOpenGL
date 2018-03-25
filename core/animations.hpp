#ifndef ANIMATIONS_H
#define ANIMATIONS_H

#include <vector>
#include <map>
#include <string>
#include <glm/glm.hpp>

#include "models.hpp"
#include "texture.hpp"

class Joint;
class Animator;
class KeyFrame;
class Animation;
class Quaternion;
class Animator;

/**
 * 
 * This class represents an entity in the world that can be animated. It
 * contains the model's VAO which contains the mesh data, the texture, and the
 * root joint of the joint hierarchy, or "skeleton". It also holds an int which
 * represents the number of joints that the model's skeleton contains, and has
 * its own {@link Animator} instance which can be used to apply animations to
 * this entity.
 * 
 * @author Karl
 *
 */
class AnimatedModel {
    private:
        VertexArray* _model;
        Texture* _texture;
        Joint* _rootJoint;
        int _jointCount;
        Animator* _animator;
        
    public:
        /**
         * Creates a new entity capable of animation. The inverse bind transform for
         * all joints is calculated in this constructor. The bind transform is
         * simply the original (no pose applied) transform of a joint in relation to
         * the model's origin (model-space). The inverse bind transform is simply
         * that but inverted.
         * 
         * @param model
         *            - the VAO containing the mesh data for this entity. This
         *            includes vertex positions, normals, texture coords, IDs of
         *            joints that affect each vertex, and their corresponding
         *            weights.
         * @param texture
         *            - the diffuse texture for the entity.
         * @param rootJoint
         *            - the root joint of the joint hierarchy which makes up the
         *            "skeleton" of the entity.
         * @param jointCount
         *            - the number of joints in the joint hierarchy (skeleton) for
         *            this entity.
         * 
         */
        AnimatedModel(VertexArray* model, Texture* texture, Joint* rootJoint, int jointCount);

        /**
         * @return The VAO containing all the mesh data for this entity.
         */
        inline VertexArray* model() const { return _model; }

        /**
         * @return The diffuse texture for this entity.
         */
        inline Texture* texture() const { return _texture; }

        /**
         * @return The root joint of the joint hierarchy. This joint has no parent,
         *         and every other joint in the skeleton is a descendant of this
         *         joint.
         */
        inline Joint* rootJoint() const { return _rootJoint; }

        /**
         * Deletes the OpenGL objects associated with this entity, namely the model
         * (VAO) and texture.
         */
        ~AnimatedModel();

        /**
         * Instructs this entity to carry out a given animation. To do this it
         * basically sets the chosen animation as the current animation in the
         * {@link Animator} object.
         * 
         * @param animation
         *            - the animation to be carried out.
         */
        void doAnimation(Animation* animation);

        /**
         * Updates the animator for this entity, basically updating the animated
         * pose of the entity. Must be called every frame.
         */
        void update();

        /**
         * Gets an array of the all important model-space transforms of all the
         * joints (with the current animation pose applied) in the entity. The
         * joints are ordered in the array based on their joint index. The position
         * of each joint's transform in the array is equal to the joint's index.
         * 
         * @return The array of model-space transforms of the joints in the current
         *         animation pose.
         */
        std::vector<glm::mat4> getJointTransforms();

        /**
         * This adds the current model-space transform of a joint (and all of its
         * descendants) into an array of transforms. The joint's transform is added
         * into the array at the position equal to the joint's index.
         * 
         * @param headJoint
         *            - the current joint being added to the array. This method also
         *            adds the transforms of all the descendents of this joint too.
         * @param jointMatrices
         *            - the array of joint transforms that is being filled.
         */
        void addJointsToArray(Joint* headJoint, std::vector<glm::mat4>& jointMatrices);

};

/**
 * 
 * Represents a joint in a "skeleton". It contains the index of the joint which
 * determines where in the vertex shader uniform array the joint matrix for this
 * joint is loaded up to. It also contains the name of the bone, and a list of
 * all the child joints.
 * 
 * The "animatedTransform" matrix is the joint transform that I keep referring
 * to in the tutorial. This is the transform that gets loaded up to the vertex
 * shader and is used to transform vertices. It is a model-space transform that
 * transforms the joint from it's bind (original position, no animation applied)
 * position to it's current position in the current pose. Changing this
 * transform changes the position/rotation of the joint in the animated entity.
 * 
 * The two other matrices are transforms that are required to calculate the
 * "animatedTransform" in the {@link Animator} class. It also has the local bind
 * transform which is the original (no pose/animation applied) transform of the
 * joint relative to the parent joint (in bone-space).
 * 
 * The "localBindTransform" is the original (bind) transform of the joint
 * relative to its parent (in bone-space). The inverseBindTransform is that bind
 * transform in model-space, but inversed.
 * 
 * @author Karl
 *
 */
class Joint {
    private:
        int _index;// ID
        std::string _name;
        std::vector<Joint*> _children;
        glm::mat4 _animatedTransform;
        
        glm::mat4 _localBindTransform;
        glm::mat4 _inverseBindTransform ;

    public:
        /**
         * @param index
         *            - the joint's index (ID).
         * @param name
         *            - the name of the joint. This is how the joint is named in the
         *            collada file, and so is used to identify which joint a joint
         *            transform in an animation keyframe refers to.
         * @param bindLocalTransform
         *            - the bone-space transform of the joint in the bind position.
         */
        Joint(int index, std::string name, glm::mat4 bindLocalTransform);
        
        inline std::vector<Joint*> children() const { return _children; }
        inline std::string name() const { return _name; }
        inline int index() const { return _index; }

        /**
         * Adds a child joint to this joint. Used during the creation of the joint
         * hierarchy. Joints can have multiple children, which is why they are
         * stored in a list (e.g. a "hand" joint may have multiple "finger" children
         * joints).
         * 
         * @param child
         *            - the new child joint of this joint.
         */
        inline void addChild(Joint* child) { _children.push_back(child); }

        /**
         * The animated transform is the transform that gets loaded up to the shader
         * and is used to deform the vertices of the "skin". It represents the
         * transformation from the joint's bind position (original position in
         * model-space) to the joint's desired animation pose (also in model-space).
         * This matrix is calculated by taking the desired model-space transform of
         * the joint and multiplying it by the inverse of the starting model-space
         * transform of the joint.
         * 
         * @return The transformation matrix of the joint which is used to deform
         *         associated vertices of the skin in the shaders.
         */
        inline glm::mat4 getAnimatedTransform() const { return _animatedTransform; }

        /**
         * This method allows those all important "joint transforms" (as I referred
         * to them in the tutorial) to be set by the animator. This is used to put
         * the joints of the animated model in a certain pose.
         * 
         * @param animationTransform - the new joint transform.
         */
        inline void setAnimationTransform(glm::mat4 animationTransform) { _animatedTransform = animationTransform; }

        /**
         * This returns the inverted model-space bind transform. The bind transform
         * is the original model-space transform of the joint (when no animation is
         * applied). This returns the inverse of that, which is used to calculate
         * the animated transform matrix which gets used to transform vertices in
         * the shader.
         * 
         * @return The inverse of the joint's bind transform (in model-space).
         */
        inline glm::mat4 getInverseBindTransform() const { return _inverseBindTransform; }

        /**
         * This is called during set-up, after the joints hierarchy has been
         * created. This calculates the model-space bind transform of this joint
         * like so: </br>
         * </br>
         * {@code bindTransform = parentBindTransform * localBindTransform}</br>
         * </br>
         * where "bindTransform" is the model-space bind transform of this joint,
         * "parentBindTransform" is the model-space bind transform of the parent
         * joint, and "localBindTransform" is the bone-space bind transform of this
         * joint. It then calculates and stores the inverse of this model-space bind
         * transform, for use when calculating the final animation transform each
         * frame. It then recursively calls the method for all of the children
         * joints, so that they too calculate and store their inverse bind-pose
         * transform.
         * 
         * @param parentBindTransform
         *            - the model-space bind transform of the parent joint.
         */
        void calcInverseBindTransform(glm::mat4 parentBindTransform);

};

class Animation {

	private:
        float _length;//in seconds
        std::vector<KeyFrame*> _keyFrames;

    public:
        /**
         * @param lengthInSeconds
         *            - the total length of the animation in seconds.
         * @param frames
         *            - all the keyframes for the animation, ordered by time of
         *            appearance in the animation.
         */
        Animation(float lengthInSeconds, std::vector<KeyFrame*> frames);

        /**
         * @return The length of the animation in seconds.
         */
        inline float length() const { return _length; }

        /**
         * @return An array of the animation's keyframes. The array is ordered based
         *         on the order of the keyframes in the animation (first keyframe of
         *         the animation in array position 0).
         */
        inline std::vector<KeyFrame*> keyFrames() const { return _keyFrames; }

};

class Animator {

	private:
        AnimatedModel* _entity;
        Animation* _currentAnimation;
        float _animationTime;

    public:
        /**
         * @param entity
         *            - the entity which will by animated by this animator.
         */
        Animator(AnimatedModel* entity);

        /**
         * Indicates that the entity should carry out the given animation. Resets
         * the animation time so that the new animation starts from the beginning.
         * 
         * @param animation
         *            - the new animation to carry out.
         */
        void doAnimation(Animation* animation);

        /**
         * This method should be called each frame to update the animation currently
         * being played. This increases the animation time (and loops it back to
         * zero if necessary), finds the pose that the entity should be in at that
         * time of the animation, and then applies that pose to all the model's
         * joints by setting the joint transforms.
         */
        void update();

    private:
        /**
         * Increases the current animation time which allows the animation to
         * progress. If the current animation has reached the end then the timer is
         * reset, causing the animation to loop.
         */
        void increaseAnimationTime();

        /**
         * This method returns the current animation pose of the entity. It returns
         * the desired local-space transforms for all the joints in a map, indexed
         * by the name of the joint that they correspond to.
         * 
         * The pose is calculated based on the previous and next keyframes in the
         * current animation. Each keyframe provides the desired pose at a certain
         * time in the animation, so the animated pose for the current time can be
         * calculated by interpolating between the previous and next keyframe.
         * 
         * This method first finds the preious and next keyframe, calculates how far
         * between the two the current animation is, and then calculated the pose
         * for the current animation time by interpolating between the transforms at
         * those keyframes.
         * 
         * @return The current pose as a map of the desired local-space transforms
         *         for all the joints. The transforms are indexed by the name ID of
         *         the joint that they should be applied to.
         */
        std::map<std::string, glm::mat4> calculateCurrentAnimationPose();

        /**
         * This is the method where the animator calculates and sets those all-
         * important "joint transforms" that I talked about so much in the tutorial.
         * 
         * This method applies the current pose to a given joint, and all of its
         * descendants. It does this by getting the desired local-transform for the
         * current joint, before applying it to the joint. Before applying the
         * transformations it needs to be converted from local-space to model-space
         * (so that they are relative to the model's origin, rather than relative to
         * the parent joint). This can be done by multiplying the local-transform of
         * the joint with the model-space transform of the parent joint.
         * 
         * The same thing is then done to all the child joints.
         * 
         * Finally the inverse of the joint's bind transform is multiplied with the
         * model-space transform of the joint. This basically "subtracts" the
         * joint's original bind (no animation applied) transform from the desired
         * pose transform. The result of this is then the transform required to move
         * the joint from its original model-space transform to it's desired
         * model-space posed transform. This is the transform that needs to be
         * loaded up to the vertex shader and used to transform the vertices into
         * the current pose.
         * 
         * @param currentPose
         *            - a map of the local-space transforms for all the joints for
         *            the desired pose. The map is indexed by the name of the joint
         *            which the transform corresponds to.
         * @param joint
         *            - the current joint which the pose should be applied to.
         * @param parentTransform
         *            - the desired model-space transform of the parent joint for
         *            the pose.
         */
        void applyPoseToJoints(std::map<std::string, glm::mat4> currentPose, Joint* joint, glm::mat4 parentTransform);

        /**
         * Finds the previous keyframe in the animation and the next keyframe in the
         * animation, and returns them in an array of length 2. If there is no
         * previous frame (perhaps current animation time is 0.5 and the first
         * keyframe is at time 1.5) then the first keyframe is used as both the
         * previous and next keyframe. The last keyframe is used for both next and
         * previous if there is no next keyframe.
         * 
         * @return The previous and next keyframes, in an array which therefore will
         *         always have a length of 2.
         */
        std::vector<KeyFrame*> getPreviousAndNextFrames();

        /**
         * Calculates how far between the previous and next keyframe the current
         * animation time is, and returns it as a value between 0 and 1.
         * 
         * @param previousFrame
         *            - the previous keyframe in the animation.
         * @param nextFrame
         *            - the next keyframe in the animation.
         * @return A number between 0 and 1 indicating how far between the two
         *         keyframes the current animation time is.
         */
        float calculateProgression(KeyFrame* previousFrame, KeyFrame* nextFrame);

        /**
         * Calculates all the local-space joint transforms for the desired current
         * pose by interpolating between the transforms at the previous and next
         * keyframes.
         * 
         * @param previousFrame
         *            - the previous keyframe in the animation.
         * @param nextFrame
         *            - the next keyframe in the animation.
         * @param progression
         *            - a number between 0 and 1 indicating how far between the
         *            previous and next keyframes the current animation time is.
         * @return The local-space transforms for all the joints for the desired
         *         current pose. They are returned in a map, indexed by the name of
         *         the joint to which they should be applied.
         */
        std::map<std::string, glm::mat4> interpolatePoses(KeyFrame* previousFrame, KeyFrame* nextFrame, float progression);

};

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
        static Quaternion interpolate(Quaternion a, Quaternion b, float blend);

};

/**
 * 
 * Represents the local bone-space transform of a joint at a certain keyframe
 * during an animation. This includes the position and rotation of the joint,
 * relative to the parent joint (for the root joint it's relative to the model's
 * origin, seeing as the root joint has no parent). The transform is stored as a
 * position vector and a quaternion (rotation) so that these values can be
 * easily interpolated, a functionality that this class also provides.
 * 
 * @author Karl
 *
 */

class JointTransform {

	// remember, this position and rotation are relative to the parent bone!
	private:
        glm::vec3 _position;
        Quaternion _rotation;

    public:
        /**
         * 
         * @param position
         *            - the position of the joint relative to the parent joint
         *            (bone-space) at a certain keyframe. For example, if this joint
         *            is at (5, 12, 0) in the model's coordinate system, and the
         *            parent of this joint is at (2, 8, 0), then the position of
         *            this joint relative to the parent is (3, 4, 0).
         * @param rotation
         *            - the rotation of the joint relative to the parent joint
         *            (bone-space) at a certain keyframe.
         */
        JointTransform(glm::vec3 position, Quaternion rotation);
        
        inline Quaternion rotation() const { return _rotation; }
        inline glm::vec3 position() const { return _position; }
        /**
         * In this method the bone-space transform matrix is constructed by
         * translating an identity matrix using the position variable and then
         * applying the rotation. The rotation is applied by first converting the
         * quaternion into a rotation matrix, which is then multiplied with the
         * transform matrix.
         * 
         * @return This bone-space joint transform as a matrix. The exact same
         *         transform as represented by the position and rotation in this
         *         instance, just in matrix form.
         */
        glm::mat4 getLocalTransform() const;

        /**
         * Interpolates between two transforms based on the progression value. The
         * result is a new transform which is part way between the two original
         * transforms. The translation can simply be linearly interpolated, but the
         * rotation interpolation is slightly more complex, using a method called
         * "SLERP" to spherically-linearly interpolate between 2 quaternions
         * (rotations). This gives a much much better result than trying to linearly
         * interpolate between Euler rotations.
         * 
         * @param frameA
         *            - the previous transform
         * @param frameB
         *            - the next transform
         * @param progression
         *            - a number between 0 and 1 indicating how far between the two
         *            transforms to interpolate. A progression value of 0 would
         *            return a transform equal to "frameA", a value of 1 would
         *            return a transform equal to "frameB". Everything else gives a
         *            transform somewhere in-between the two.
         * @return
         */
        static JointTransform* interpolate(JointTransform& frameA, JointTransform& frameB, float progression);

    private:
        /**
         * Linearly interpolates between two translations based on a "progression"
         * value.
         * 
         * @param start
         *            - the start translation.
         * @param end
         *            - the end translation.
         * @param progression
         *            - a value between 0 and 1 indicating how far to interpolate
         *            between the two translations.
         * @return
         */
        static glm::vec3 interpolate(glm::vec3 start, glm::vec3 end, float progression);

};

/**
 * 
 * Represents one keyframe of an animation. This contains the timestamp of the
 * keyframe, which is the time (in seconds) from the start of the animation when
 * this keyframe occurs.
 * 
 * It also contains the desired bone-space transforms of all of the joints in
 * the animated entity at this keyframe in the animation (i.e. it contains all
 * the joint transforms for the "pose" at this time of the animation.). The
 * joint transforms are stored in a map, indexed by the name of the joint that
 * they should be applied to.
 * 
 * @author Karl
 *
 */
class KeyFrame {

	private:
        float _timeStamp;
        std::map<std::string, JointTransform*> _pose;

    public:
	/**
	 * @param timeStamp
	 *            - the time (in seconds) that this keyframe occurs during the
	 *            animation.
	 * @param jointKeyFrames
	 *            - the local-space transforms for all the joints at this
	 *            keyframe, indexed by the name of the joint that they should be
	 *            applied to.
	 */
	KeyFrame(float timeStamp, std::map<std::string, JointTransform*> jointKeyFrames);

	/**
	 * @return The time in seconds of the keyframe in the animation.
	 */
	inline float timeStamp() const { return _timeStamp; }

	/**
	 * @return The desired bone-space transforms of all the joints at this
	 *         keyframe, of the animation, indexed by the name of the joint that
	 *         they correspond to. This basically represents the "pose" at this
	 *         keyframe.
	 */
	inline std::map<std::string, JointTransform*> jointKeyFrames() const { return _pose; }

};

#endif
