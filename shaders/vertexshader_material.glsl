#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in ivec4 BoneIDs;
layout (location = 4) in vec4 Weights;
layout (location = 5) in vec3 tangents;
layout (location = 6) in vec3 bitangents;
//~ layout (location = 3) in int BoneIDs;
//~ layout (location = 4) in float Weights;

in vec3 LightDirection_cameraspace;

out vec2 TexCoords;
out vec3 FragPos;
out vec3 Normal;
out vec3 PosEyeSpace;

const int MAX_BONES = 100;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 gBones[MAX_BONES];

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Light light;

void preBillPhong() {
    //Calculating the vertex position into eyespace
    PosEyeSpace = ((view* model) * vec4(aPos, 1.0)).xyz;
    
    //Normal to be passed to the fragment shder 
    Normal = mat3(transpose(inverse(model))) * aNormal;  
}

void main() 
{

    mat4 BoneTransform = mat4(1.);
    BoneTransform += gBones[BoneIDs[0]] * Weights[0];
    BoneTransform += gBones[BoneIDs[1]] * Weights[1];
    BoneTransform += gBones[BoneIDs[2]] * Weights[2];
    BoneTransform += gBones[BoneIDs[3]] * Weights[3];
    
    FragPos = vec3(model * vec4(aPos, 1.0));
    
    //~ Normal = (BoneTransform * vec4(aNormal, 0.0)).xyz; 
    
    
    
    vec4 PosL      = BoneTransform * vec4(aPos, 1.0);
    //~ vec4 PosL      = vec4(aPos, 1.0);
    
    mat4 mvp = projection * view * model;
    
    //The position of the vertex
    gl_Position = mvp * PosL;
    
    preBillPhong();
    
    // Position of the vertex, in worldspace : M * position
    //worldPosition = (M * vec4(modelVertex,1)).xyz;
    
    
    //~ gl_Position = mvp * vec4(aPos, 1.0);
    //~ gl_Position = mvp * (BoneTransform * vec4(aPos,1));
    //~ gl_Position = projection * view * model * vec4(aPos,1);
    
    
    //~ gl_Position = projection * view * model * vec4(aPos,1);
    //~ gl_Position = projection * view * model *vec4(aPos,1);

    TexCoords = aTexCoords;    
}
