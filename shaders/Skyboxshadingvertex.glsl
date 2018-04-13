#version 330

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in ivec4 BoneIDs;
layout (location = 4) in vec4 Weights;

//layout(location = 0) in vec3 vertexPosition_modelspace;
//layout(location = 1) in vec2 vertexUV;

//in vec3 vp;
out vec3 texcoords;

// Values that stay constant for the whole mesh.
//uniform mat4 MVP;
//uniform mat4 V; //Special View, doens't have camera transformations
//uniform mat4 M;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

//uniform vec3 LightPosition_worldspace;

void main() {
  texcoords = vec3(aPos.x, aPos.y, aPos.z);
  //texcoords = vec3(aTexCoords, aPos.z);
  mat4 MVP = projection * view * model;
  gl_Position = MVP * vec4(aPos, 1.0);

  //texcoords = normalize(vec3(a_position.x, a_position.y, a_position.z));
         //v_cubemapTexture = normalize(texture_pos.xyz); 
  //~ gl_Position =  MVP * vec4(aPos,1);
}
