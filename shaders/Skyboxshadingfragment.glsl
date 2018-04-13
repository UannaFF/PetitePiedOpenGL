#version 330

in vec3 texcoords;
uniform samplerCube texture_cube;
out vec4 frag_colour;

void main() {
  frag_colour = texture(texture_cube, texcoords);
  //if((frag_colour.x > 1.0) || (frag_colour.x < 0.0)) frag_colour.x = 1.0;
  //if(frag_colour.y > 1.0 || frag_colour.y < 0.0) frag_colour.y = 1.0;
  //if(frag_colour.z > 1.0 || frag_colour.z < 0.0) frag_colour.z = 1.0;
  /*if(texcoords.x == 0.0) {
  	frag_colour = vec4(1.0,0.0,0.0, 1.0);
  } else {
  	frag_colour = vec4(0.0,1.0,0.0, 1.0);
  }*/
}
