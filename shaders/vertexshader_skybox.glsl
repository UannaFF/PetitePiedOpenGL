#version 330

in vec3 texcoords;
uniform samplerCube texture_cube;
out vec4 frag_colour;

void main() {
  frag_colour = texture(texture_cube, texcoords);
}
