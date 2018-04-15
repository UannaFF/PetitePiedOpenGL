#version 330 core

out vec4 FragColor;
in vec2 TexCoords;
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_height1; //normal

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;    
    float shininess;
}; 

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 FragPos;  
in vec3 Normal;
in vec3 PosEyeSpace;
  
uniform vec3 viewPos;
uniform Material material;
uniform Light light;
uniform Light light2;


//Phong shading for only one light source
vec4 phongShading(vec4 texColor) {
    
    vec4 res = vec4(1.0);
    
    vec3 Normal = normalize(Normal);
    vec3 L = normalize(light.position.xyz - PosEyeSpace); //light vector 
    vec3 E = normalize(-PosEyeSpace); // we are in Eye Coordinates, so EyePos is (0,0,0)  
    vec3 R = normalize(-reflect(L,Normal));
    
    //Ambient component
    vec3 ambient = light.ambient * material.ambient;
    
    //Diffuse component
    vec3 diffuse = light.diffuse * material.diffuse * max(dot(L, Normal), 0.0);
    
    //Specular component
    vec3 specular = light.specular * material.specular * pow(max(dot(R, E), 0.0), 0.3*material.shininess);
    
    //return vec4(ambient + diffuse, 1) * texColor + vec4(specular, 1);
    return vec4(texColor.xyz + ambient + diffuse + specular, 1.0);
}

vec4 bumpMapping() {
    return vec4(1.0);
}

void main()
{
    
    //Starting bump map//////////////
        
    // Local normal, in tangent space. Expanding the range of the normal space
    /*vec3 tangentNormal = normalize(texture2D(texture_height1, TexCoords).rgb*2.0 - 1.0);
        // Calculate the lighting diffuse value  
    float diffuse = max(dot(tangentNormal, light.position), 0.0);
    FragColor = texture(texture_diffuse1, TexCoords) * diffuse;*/
    
    ////////////////////////////////
    
    ///////////////////////////////
    FragColor = phongShading(texture(texture_diffuse1, TexCoords));
    //////////////////////////////
    
    // ambient
    /*vec3 ambient = light.ambient * material.ambient;
  	
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * (diff * material.diffuse);
    
    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * (spec * material.specular);  
        
    vec3 result = ambient + diffuse + specular;
    //FragColor = vec4(result, 1.0);
    FragColor = texture(texture_diffuse1, TexCoords) + texture(texture_specular1, TexCoords);*/

} 


