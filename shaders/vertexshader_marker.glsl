#version 330 core
layout(location = 0) in vec3 position; 

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
out vec3 fragColor;

void main() {
    gl_Position = projection * view * model * vec4(position, 1);
    fragColor = vec3(abs(position.x), abs(position.y), abs(position.z));
}
