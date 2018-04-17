#include "camera.hpp"
#include "window.hpp"
#include "common.hpp"

using namespace glm;

Camera::Camera(Window* p):
    _parent(p), _projection_fragment(-1), _view_fragment(-1), 
    _viewMatrix(1.f), _projectionMatrix(1.f)
{
}

void Camera::setViewMatrix(glm::mat4 v){
    _viewMatrix = v;
}

void Camera::setProjectionMatrix(glm::mat4 p){
    _projectionMatrix = p;
}
        

ControlableCamera::ControlableCamera(Window*p, bool restricted):
    Camera::Camera(p),
    _position(-3.61, 2, 0.23), _horizontalAngle(glm::radians(-180.)), _verticalAngle(glm::radians(-90.)), _initialFoV(45.0f),
    _speed(1.0f), _mouseSpeed(0.005f), _restricted_box(restricted)
{}

void ControlableCamera::updateFromMouse(){
    if (!parent())
        throw new OpenGLException("Camera has no parent window.", 0);
    
	// glfwGetTime is called only once, the first time this function is called
	static double lastTime = glfwGetTime();

	// Compute time difference between current and last frame
    double currentTime = glfwGetTime();
	float deltaTime = float(currentTime - lastTime);

	// Get mouse position
	double xpos, ypos;
	glfwGetCursorPos(parent()->internal(), &xpos, &ypos);

	// Reset mouse position for next frame
	glfwSetCursorPos(parent()->internal(), parent()->width()/2, parent()->height()/2);

	// Compute new orientation
    double incr = _horizontalAngle + _mouseSpeed * float(parent()->width()/2 - xpos );
    _horizontalAngle = incr;
	//_horizontalAngle += _mouseSpeed * float(parent()->width()/2 - xpos ); // Theta
    //if(_horizontalAngle < -90) _horizontalAngle = -90;
    //if(_horizontalAngle < 90) _horizontalAngle = 90;
	_verticalAngle   -= _mouseSpeed * float(parent()->height()/2 - ypos ); // Phy
	
    // Reset cam
	if (glfwGetKey( parent()->internal(), GLFW_KEY_P ) == GLFW_PRESS){
        _horizontalAngle = glm::radians(214.); // Theta
        _verticalAngle   = glm::radians(-260.); // Phy
        _position = glm::vec3(7.0, 4.6, 2.29);
	}

	// Direction : Spherical coordinates to Cartesian coordinates conversion
	glm::vec3 direction(
		sin(_verticalAngle) * cos(_horizontalAngle), 
		sin(_verticalAngle) * sin(_horizontalAngle),
		cos(_verticalAngle)
	);
	
	// Right vector
	glm::vec3 right = glm::vec3(
		cos(_horizontalAngle - 3.14f/2.0f),
		sin(_horizontalAngle - 3.14f/2.0f), 
		0
	);
	
	// Up vector
	glm::vec3 up = glm::cross( right, direction );
    
    float speed = _speed;
	// Speed up
	if (glfwGetKey( parent()->internal(), GLFW_KEY_LEFT_SHIFT ) == GLFW_PRESS || glfwGetKey( parent()->internal(), GLFW_KEY_RIGHT_SHIFT ) == GLFW_PRESS)
        speed *= 15;

	// Move forward
	if (glfwGetKey( parent()->internal(), GLFW_KEY_UP ) == GLFW_PRESS){
		_position += direction * deltaTime * speed;
	}
	// Move backward
	if (glfwGetKey( parent()->internal(), GLFW_KEY_DOWN ) == GLFW_PRESS){
		_position -= direction * deltaTime * speed;
	}
	// Strafe right
	if (glfwGetKey( parent()->internal(), GLFW_KEY_RIGHT ) == GLFW_PRESS){
		_position += right * deltaTime * speed;
	}
	// Strafe left
	if (glfwGetKey( parent()->internal(), GLFW_KEY_LEFT ) == GLFW_PRESS){
		_position -= right * deltaTime * speed;
	}
    
    if (_restricted_box){
        if(_position.x >= 3.885)
            _position.x = 3.885;
        if(_position.x <= 3.84)
            _position.x = 3.84;
        if(_position.y <= 1.8)
            _position.y = 1.8;
        if(_position.y >= 2.85)
            _position.y = 2.8;
        if(_position.z >= 0.2)
            _position.z = 0.2;
        if(_position.z <= 0.13)
            _position.z = 0.13;
    }
    
    DEBUG(Debug::Info, "pos: %f %f %f, ha: %f, va: %f\n", _position.x, _position.y, _position.z, _horizontalAngle, _verticalAngle);
    
	float FoV = _initialFoV;// - 5 * glfwGetMouseWheel(); 
    

	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	setProjectionMatrix(glm::perspective(glm::radians(FoV), 4.0f / 3.0f, 0.1f, 100.0f));
	// Camera matrix
	setViewMatrix(glm::lookAt(
								_position,           // Camera is here
								_position+direction, // and looks here : at the same position, plus "direction"
								up                  // Head is up (set to 0,-1,0 to look upside-down)
						   ));

	// For the next frame, the "last time" will be "now"
	lastTime = currentTime;
}
