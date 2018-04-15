#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in ivec4 BoneIDs;
layout (location = 4) in vec4 Weights;
layout (location = 5) in vec3 aTangent;
layout (location = 6) in vec3 aBitangent;
//~ layout (location = 3) in int BoneIDs;
//~ layout (location = 4) in float Weights;

in vec3 LightDirection_cameraspace;

out vec2 TexCoords;
out vec3 FragPos;
out vec3 Normal;
out vec3 PosEyeSpace;
out vec3 lightDirection_tangentspace;
out vec3 eyeDirection_tangentspace;

const int MAX_BONES = 100;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 gBones[MAX_BONES];
mat4 MIT = mat4(1.0); //Inverse transpose of the model

mat4 mv = mat4(1.0);
vec3 aNormal_cameraspace;
vec3 aTangent_cameraspace;
vec3 aBitangent_cameraspace;
mat3 TBN;

uniform float type;

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Light light;

void preBillPhong() {
    mv = view* model;
    MIT = transpose(inverse(model));
    //Calculating the vertex position into eyespace
    PosEyeSpace = (view * model * vec4(aPos, 1.0)).xyz;
    
    //Normal to be passed to the fragment shder 
    Normal = mat3(inverse(model)) * aNormal;  
}

void fromCameraToTangent() {
    
    MIT = transpose(inverse(model));
    mv = view* MIT;
    //we need to use the inverse transpose to take into account transformations
    
    aNormal_cameraspace = mat3(mv) * normalize(aNormal);
    aTangent_cameraspace = mat3(mv) * normalize(aTangent);
    aBitangent_cameraspace = mat3(mv) * normalize(aBitangent);
    
    //This matrix takes us from camera sace to tanent space
    TBN = inverse(mat3(aTangent, aBitangent, aNormal));
    
    vec4 v        = model * vec4(aPos, 1.0);                  // vertex in model space
    lightDirection_tangentspace  = TBN * (inverse(model) * vec4(light.position - v.xyz, 0)).xyz;  // light vector in tangent space
    
    v = view * model * vec4(aPos, 1.0);                       // vertex in eye space
    vec4 light_vector_eye  = normalize((view * vec4(light.position, 1.0)) - v); // light vector in eye space
    vec4 viewer_vector_eye = normalize(-v);                     // view vector in eye space
 
    eyeDirection_tangentspace = TBN * (inverse(view * model) * vec4((light_vector_eye.xyz + viewer_vector_eye.xyz), 0.0)).xyz;
                                            // halfway vector in tangent space
                                            
    Normal = inverse(transpose(TBN)) * aNormal;

}

void main() 
{
    
    mv = view* model;
    MIT = transpose(inverse(model));

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
    
    if(type == 1.0) preBillPhong();
    else if(type == 2.0) fromCameraToTangent();
    
    
    //~ gl_Position = mvp * vec4(aPos, 1.0);
    //~ gl_Position = mvp * (BoneTransform * vec4(aPos,1));
    //~ gl_Position = projection * view * model * vec4(aPos,1);
    
    
    //~ gl_Position = projection * view * model * vec4(aPos,1);
    //~ gl_Position = projection * view * model *vec4(aPos,1);

    TexCoords = aTexCoords;    
}
