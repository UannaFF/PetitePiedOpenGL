#include "camera.hpp"
#include "window.hpp"

using namespace glm;

Camera::Camera(Window* p):
    _parent(p), _projection_fragment(-1), _view_fragment(-1), 
    _viewMatrix(1.f), _projectionMatrix(1.f)
{
}

void Camera::setViewMatrix(glm::mat4 v){
    _viewMatrix = v;
    if (_view_fragment > -1)
        glUniformMatrix4fv(_view_fragment, 1, GL_FALSE, &v[0][0]);
}

void Camera::setProjectionMatrix(glm::mat4 p){
    _projectionMatrix = p;
    if (_projection_fragment > -1)
        glUniformMatrix4fv(_projection_fragment, 1, GL_FALSE, &p[0][0]);
}
        

ControlableCamera::ControlableCamera():
    Camera::Camera(),
    _position(1.f), _horizontalAngle(3.14f), _verticalAngle(0.0f), _initialFoV(45.0f),
    _speed(3.0f), _mouseSpeed(0.005f){
}

void ControlableCamera::updateFromMouse(){
    if (!parent())
        throw new OpenGLException("Camera has no parent window.");
    
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
	_horizontalAngle += _mouseSpeed * float(parent()->width()/2 - xpos );
	_verticalAngle   += _mouseSpeed * float(parent()->height()/2 - ypos );

	// Direction : Spherical coordinates to Cartesian coordinates conversion
	glm::vec3 direction(
		cos(_verticalAngle) * sin(_horizontalAngle), 
		sin(_verticalAngle),
		cos(_verticalAngle) * cos(_horizontalAngle)
	);
	
	// Right vector
	glm::vec3 right = glm::vec3(
		sin(_horizontalAngle - 3.14f/2.0f), 
		0,
		cos(_horizontalAngle - 3.14f/2.0f)
	);
	
	// Up vector
	glm::vec3 up = glm::cross( right, direction );

	// Move forward
	if (glfwGetKey( parent()->internal(), GLFW_KEY_UP ) == GLFW_PRESS){
		_position += direction * deltaTime * _speed;
	}
	// Move backward
	if (glfwGetKey( parent()->internal(), GLFW_KEY_DOWN ) == GLFW_PRESS){
		_position -= direction * deltaTime * _speed;
	}
	// Strafe right
	if (glfwGetKey( parent()->internal(), GLFW_KEY_RIGHT ) == GLFW_PRESS){
		_position += right * deltaTime * _speed;
	}
	// Strafe left
	if (glfwGetKey( parent()->internal(), GLFW_KEY_LEFT ) == GLFW_PRESS){
		_position -= right * deltaTime * _speed;
	}

	float FoV = _initialFoV;// - 5 * glfwGetMouseWheel(); // Now GLFW 3 requires setting up a callback for this. It's a bit too complicated for this beginner's tutorial, so it's disabled instead.

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
        update();
}
