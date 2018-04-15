#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in ivec4 BoneIDs;
layout (location = 4) in vec4 Weights;
//~ layout (location = 3) in int BoneIDs;
//~ layout (location = 4) in float Weights;

in vec3 LightDirection_cameraspace;

out vec2 TexCoords;
out vec3 FragPos;
out vec3 Normal;

const int MAX_BONES = 100;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 gBones[MAX_BONES];

void main()
{

    //~ mat4 BoneTransform = mat4(1.);
    mat4 BoneTransform = gBones[BoneIDs.x] * Weights.x;
    BoneTransform += gBones[BoneIDs.y] * Weights.y;
    BoneTransform += gBones[BoneIDs[2]] * Weights[2];
    BoneTransform += gBones[BoneIDs[3]] * Weights[3];
    
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = (BoneTransform * vec4(aNormal, 0.0)).xyz;  
    //~ Normal = mat3(transpose(inverse(model))) * aNormal;
    
    vec4 PosL      = BoneTransform * vec4(aPos, 1.0);
    //~ vec4 PosL      = vec4(aPos, 1.0);
    mat4 mvp = projection * view * model;
    
    vec4 NormalL = BoneTransform * vec4(aNormal, 0.0);
    Normal = (model * NormalL).xyz;
    
    gl_Position = mvp * PosL;
    //~ gl_Position = mvp * vec4(aPos, 1.0);
    //~ gl_Position = mvp * (BoneTransform * vec4(aPos,1));
    //~ gl_Position = projection * view * model * vec4(aPos,1);
    
    
    //~ gl_Position = projection * view * model * vec4(aPos,1);
    //~ gl_Position = projection * view * model *vec4(aPos,1);

    TexCoords = aTexCoords;    
}
