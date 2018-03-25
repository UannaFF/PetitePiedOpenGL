#ifndef CAMERA_H
#define CAMERA_H

extern "C" {
#include <GL/glew.h>
#include <GLFW/glfw3.h>
}
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <exception>
#include <string>

#include "common.hpp"

class Window;

class Camera {
    public:
        Camera(Window*p = nullptr);
        
        inline void setParent(Window* p){ _parent = p; }
        inline Window* parent() const { return _parent; }

        inline glm::mat4 viewMatrix() const { return _viewMatrix; }
        inline void setViewMatrix(glm::mat4 v) { _viewMatrix = v; }
        inline glm::mat4 projectionMatrix() const { return _projectionMatrix; }
        inline void setProjectionMatrix(glm::mat4 p) { _projectionMatrix = p; }
        
    private:
        Window* _parent;
        
        glm::mat4 _viewMatrix;
        glm::mat4 _projectionMatrix;
};


class ControlableCamera: public Camera {
    public:
        ControlableCamera();
        void updateFromMouse();
        
        inline void setPosition(glm::vec3 p){ _position = p; }
        inline glm::vec3 position() const { return _position; }
        
        inline void setHorizontalAngle(float h){ _horizontalAngle = h; }
        inline float horizontalAngle() const { return _horizontalAngle; }
        
        inline void setVerticalAngle(float v){ _verticalAngle = v; }
        inline float verticalAngle() const { return _verticalAngle; }
        
        inline void setInitialFoV(float v){ _initialFoV = v; }
        inline float initialFoV() const { return _initialFoV; }
        
        inline void setMouseSensibility(float v){ _mouseSpeed = v; }
        inline float mouseSensibility() const { return _mouseSpeed; }
        
        inline void setSpeed(float v){ _speed = v; }
        inline float speed() const { return _speed; }
        
    private:
        glm::vec3 _position; 
        // Initial horizontal angle : toward -Z
        float _horizontalAngle;
        // Initial vertical angle : none
        float _verticalAngle;
        // Initial Field of View
        float _initialFoV;

        float _speed; // 3 units / second
        float _mouseSpeed;
};

#endif
