#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in ivec4 BoneIDs;
layout (location = 4) in vec4 Weights;
layout (location = 5) in vec3 aTangent;
layout (location = 6) in vec3 aBitangent;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 color;

void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0);
}


