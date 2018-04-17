#version 330

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec3 texcoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
  texcoords = aPos;
  mat4 MVP = projection * view * model;
  gl_Position = MVP * vec4(aPos, 1.0);
}
