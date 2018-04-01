#include "animations.hpp"

#include <math.h>
#include "glm/gtc/matrix_transform.hpp"

#include "models.hpp"
#include "scene.hpp"


Joint::Joint(int index, std::string name, glm::mat4 bindLocalTransform):
    _index(index),
    _name(name),
    _children(),
    _animatedTransform(),
    _localBindTransform(bindLocalTransform),
    _inverseBindTransform()
{
}

void Joint::calcInverseBindTransform(glm::mat4 parentBindTransform) {
    glm::mat4 bindTransform = parentBindTransform * _localBindTransform;
    
    _inverseBindTransform = glm::inverse(bindTransform);
            
    for (Joint* child : _children) {
        child->calcInverseBindTransform(bindTransform);
    }
}
int Joint::size() const {
    int res = 1;
    for (Joint* j: _children)
        res += j->size();
    return res;
}

Animation::Animation(float lengthInSeconds, std::vector<KeyFrame*> frames):
    _keyFrames(frames),
    _length(lengthInSeconds)
{
}

NodeAnimator::NodeAnimator(Node* entity):
    _entity(entity)
{
}

void NodeAnimator::doAnimation(Animation* animation) {
    _animationTime = 0;
    _currentAnimation = animation;
}

void NodeAnimator::update() {
    if (_currentAnimation == nullptr)
        return;
    increaseAnimationTime();
    std::map<std::string, glm::mat4> currentPose = calculateCurrentAnimationPose();
    applyPoseToJoints(currentPose, _entity->mesh()->rootBone(), glm::mat4(0));
}

void NodeAnimator::increaseAnimationTime() {
    _animationTime += glfwGetTime();
    if (_animationTime > _currentAnimation->length())
        _animationTime = std::fmod(_animationTime, _currentAnimation->length());
}

std::map<std::string, glm::mat4> NodeAnimator::calculateCurrentAnimationPose() {
    std::pair<KeyFrame*, KeyFrame*> frames = getPreviousAndNextFrames();
    float progression = calculateProgression(frames.first, frames.second);
    return interpolatePoses(frames.first, frames.second, progression);
}

void NodeAnimator::applyPoseToJoints(std::map<std::string, glm::mat4> currentPose, Joint* joint, glm::mat4 parentTransform) {
    glm::mat4 currentLocalTransform = currentPose.find(joint->name())->second;
    glm::mat4 currentTransform = parentTransform * currentLocalTransform;
    for (Joint* childJoint : joint->children())
        applyPoseToJoints(currentPose, childJoint, currentTransform);
    currentTransform *= joint->getInverseBindTransform();
    joint->setAnimationTransform(currentTransform);
}

std::pair<KeyFrame*, KeyFrame*> NodeAnimator::getPreviousAndNextFrames() {
    std::vector<KeyFrame*> allFrames = _currentAnimation->keyFrames();
    KeyFrame* previousFrame = allFrames[0];
    KeyFrame* nextFrame = allFrames[0];
    for (int i = 1; i < allFrames.size(); i++) {
        nextFrame = allFrames[i];
        if (nextFrame->timeStamp() > _animationTime)
            break;
        previousFrame = allFrames[i];
    }
    return std::pair<KeyFrame*, KeyFrame*>(previousFrame, nextFrame);
}

float NodeAnimator::calculateProgression(KeyFrame* previousFrame, KeyFrame* nextFrame) {
    float totalTime = nextFrame->timeStamp() - previousFrame->timeStamp();
    float currentTime = _animationTime - previousFrame->timeStamp();
    return currentTime / totalTime;
}

std::map<std::string, glm::mat4> NodeAnimator::interpolatePoses(KeyFrame* previousFrame, KeyFrame* nextFrame, float progression) {
    std::map<std::string, glm::mat4> currentPose;
    for (auto& joinPair : previousFrame->jointKeyFrames()) {
        JointTransform* previousTransform = previousFrame->jointKeyFrames().find(joinPair.first)->second;
        JointTransform* nextTransform = nextFrame->jointKeyFrames().find(joinPair.first)->second;
        JointTransform* currentTransform = JointTransform::interpolate(*previousTransform, *nextTransform, progression);
        currentPose.insert(std::pair<std::string, glm::mat4>(joinPair.first, currentTransform->getLocalTransform()));
    }
    return currentPose;
}

JointTransform::JointTransform(glm::vec3* position, Quaternion* rotation):
    _position(position),
    _rotation(rotation){
}

JointTransform::~JointTransform(){
    delete _position;
    delete _rotation;
}

glm::mat4 JointTransform::getLocalTransform() const {
    glm::mat4 res;
    if (_position){
        if (_rotation)
             return glm::translate(glm::mat4(0), *_position) * _rotation->toRotationMatrix();
        return glm::translate(glm::mat4(0), *_position);
    }
    else
        return _rotation->toRotationMatrix();
}

JointTransform* JointTransform::interpolate(JointTransform& frameA, JointTransform& frameB, float progression) {
    JointTransform* j = new JointTransform();
    j->position(new glm::vec3(interpolate(frameA.position(), frameB.position(), progression)));
    j->rotation(new Quaternion(Quaternion::interpolate(frameA.rotation(), frameB.rotation(), progression)));
    return j;
}

glm::vec3 JointTransform::interpolate(glm::vec3 start, glm::vec3 end, float progression) {
    float x = start.x + (end.x - start.x) * progression;
    float y = start.y + (end.y - start.y) * progression;
    float z = start.z + (end.z - start.z) * progression;
    return glm::vec3(x, y, z);
}

KeyFrame::KeyFrame(float timeStamp, std::map<std::string, JointTransform*> jointKeyFrames):
    _timeStamp(timeStamp),
    _pose(jointKeyFrames)
{
}

Quaternion::Quaternion(float x, float y, float z, float w):
    _x(x),
    _y(y),
    _z(z),
    _w(w)
{
    normalize();
}

void Quaternion::normalize() {
    float mag = sqrt(_w * _w + _x * _x + _y * _y + _z * _z);
    _w /= mag;
    _x /= mag;
    _y /= mag;
    _z /= mag;
}

glm::mat4 Quaternion::toRotationMatrix() const {
    glm::mat4 matrix(0);
    float xy = _x * _y, xz = _x * _z, xw = _x * _w, yz = _y * _z, yw = _y * _w, zw = _z * _w;
    float xSquared = _x * _x, ySquared = _y * _y, zSquared = _z * _z;
    matrix[0][0] = 1 - 2 * (ySquared + zSquared);
    matrix[0][1] = 2 * (xy - zw);
    matrix[0][2] = 2 * (xz + yw);
    //~ matrix[0][3] = 0;
    matrix[1][0] = 2 * (xy + zw);
    matrix[1][1] = 1 - 2 * (xSquared + zSquared);
    matrix[1][2] = 2 * (yz - xw);
    //~ matrix[1][3] = 0;
    matrix[2][0] = 2 * (xz - yw);
    matrix[2][1] = 2 * (yz + xw);
    matrix[2][2] = 1 - 2 * (xSquared + ySquared);
    //~ matrix[2][3] = 0;
    //~ matrix[3][0] = 0;
    //~ matrix[3][1] = 0;
    //~ matrix[3][2] = 0;
    matrix[3][3] = 1;
    return matrix;
}

Quaternion Quaternion::fromMatrix(glm::mat4& matrix) {
    float w, x, y, z;
    float diagonal = matrix[0][0] + matrix[1][1] + matrix[2][2];
    if (diagonal > 0) {
        float w4 = sqrt(diagonal + 1) * 2;
        w = w4 / 4;
        x = (matrix[2][1] - matrix[1][2]) / w4;
        y = (matrix[0][2] - matrix[2][0]) / w4;
        z = (matrix[1][0] - matrix[0][1]) / w4;
    } else if ((matrix[0][0] > matrix[1][1]) && (matrix[0][0] > matrix[2][2])) {
        float x4 = sqrt(1 + matrix[0][0] - matrix[1][1] - matrix[2][2]) * 2;
        w = (matrix[2][1] - matrix[1][2]) / x4;
        x = x4 / 4;
        y = (matrix[0][1] + matrix[1][0]) / x4;
        z = (matrix[0][2] + matrix[2][0]) / x4;
    } else if (matrix[1][1] > matrix[2][2]) {
        float y4 = sqrt(1 + matrix[1][1] - matrix[0][0] - matrix[2][2]) * 2;
        w = (matrix[0][2] - matrix[2][0]) / y4;
        x = (matrix[0][1] + matrix[1][0]) / y4;
        y = y4 / 4;
        z = (matrix[1][2] + matrix[2][1]) / y4;
    } else {
        float z4 = sqrt(1 + matrix[2][2] - matrix[0][0] - matrix[1][1]) * 2;
        w = (matrix[1][0] - matrix[0][1]) / z4;
        x = (matrix[0][2] + matrix[2][0]) / z4;
        y = (matrix[1][2] + matrix[2][1]) / z4;
        z = z4 / 4;
    }
    return Quaternion(x, y, z, w);
}

Quaternion Quaternion::interpolate(Quaternion a, Quaternion b, float blend) {
    Quaternion result(0, 0, 0, 1);
    float dot = a.w() * b.w() + a.x() * b.x() + a.y() * b.y() + a.z() * b.z();
    float blendI = 1 - blend;
    if (dot < 0) {
        result.w(blendI * a.w() + blend * -b.w());
        result.x(blendI * a.x() + blend * -b.x());
        result.y(blendI * a.y() + blend * -b.y());
        result.z(blendI * a.z() + blend * -b.z());
    } else {
        result.w(blendI * a.w() + blend * b.w());
        result.x(blendI * a.x() + blend * b.x());
        result.y(blendI * a.y() + blend * b.y());
        result.z(blendI * a.z() + blend * b.z());
    }
    result.normalize();
    return result;
}
