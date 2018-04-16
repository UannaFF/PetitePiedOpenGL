#version 330 core

out vec4 FragColor;
in vec2 TexCoords;
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_height1; //normal

uniform int texture_diffuse1_passed;
uniform int texture_specular1_passed;
uniform int texture_height1_passed; 

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
in vec3 lightDirection_tangentspace;
in vec3 eyeDirection_tangentspace;

  
uniform vec3 viewPos;
uniform Material material;
uniform Light light;
uniform int type;

vec4 basicTextured() {
    vec3 ambient = light.ambient * material.ambient;
    vec3 diffuse = light.diffuse * (0.5 * material.diffuse);
    if(texture_diffuse1_passed == 1)
        diffuse *= texture(texture_diffuse1, TexCoords).xyz;
    
    return vec4(ambient + diffuse, 1.0);
}

vec4 basicLight() {
    
    vec3 l = normalize(light.position.xyz - PosEyeSpace); //light vector
    float cosTheta = clamp( dot( Normal,l ), 0,1 );
    return vec4(material.diffuse * light.diffuse * cosTheta / (1*1), 1.0);
}

//Phong shading for only one light source
vec4 phongShading(vec4 texColor) {
    
    vec3 Normal = normalize(Normal);
    vec3 L = normalize(light.position.xyz - PosEyeSpace); //light vector 
    vec3 E = normalize(-PosEyeSpace); // we are in Eye Coordinates, so EyePos is (0,0,0)  
    vec3 R = normalize(-reflect(L,Normal));
    
    float d = dot(L, Normal);
    bool facing = d > 0.0;
    
    //Ambient component
    vec3 ambient = light.ambient * material.ambient;
    
    //Diffuse component
    vec3 diffuse = light.diffuse * material.diffuse * max(dot(L, Normal), 0.0);
    
    //Specular component
    vec3 specular = (facing ? light.specular * material.specular * pow(max(dot(R, E), 0.0), material.shininess) : vec3(0.0));
    
    
    //return vec4(ambient + diffuse, 1) * texColor + vec4(specular, 1);
    if(texture_diffuse1_passed == 1)
        return vec4(texColor.xyz + ambient + diffuse + specular, 1.0);
    else
        return vec4(ambient + diffuse + specular, 1.0);
        //return vec4(1.0);
}

vec4 phongShadingBumpMapping(vec4 texColor, vec3 lightDir, vec3 eyeDir, vec3 normal, vec3 normalvec) {
    
    lightDir = normalize(lightDir);
    eyeDir = normalize(eyeDir);
    
    //Normal = flag ? normalize(n.xyz+normal1) : normal1;
    normal = normalize(normal+normalvec);
    vec3 R = normalize(-reflect(lightDir,normal));
    
    //return vec4(normal_tangentspace, 1.0);
    //vec3 R = normalize(-reflect(lightDir, normal));
    float d = dot(lightDir, normal);
    bool facing = d > 0.0;
    
    //Ambient component
    vec3 ambient = light.ambient * material.ambient;
    
    //Diffuse component
    vec3 diffuse = light.diffuse * material.diffuse * max(d, 0.0);
    
    //Specular component
    //vec3 specular = (facing ? light.specular * material.specular * pow(max(dot(R, eyeDir), 0.0), material.shininess) : vec3(0.0));
    vec4 specmap = texture(texture_specular1, TexCoords);
    vec3 specular = clamp(light.specular * material.specular * pow(max(dot(R, eyeDir), 0.0), material.shininess) * specmap.xyz, 0, 1);
    
    //return vec4(ambient + diffuse, 1) * texColor + vec4(specular, 1);
    return vec4(texColor.xyz + ambient + diffuse + specular, 1.0);
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
    //FragColor = phongShading(texture(texture_diffuse1, TexCoords));
    
    //Without bumpmapping
    //~ vec3 Normal = normalize(Normal);
    //~ vec3 L = normalize(light.position.xyz - PosEyeSpace); //light vector 
    //~ vec3 E = normalize(-PosEyeSpace); // we are in Eye Coordinates, so EyePos is (0,0,0)
    
    //~ //Bump mapping
    //~ vec3 normal_tangentspace = normalize((texture( texture_height1, TexCoords ).rgb-0.5) * 2.0);
    //~ //FragColor = phongShadingBumpMapping(texture(texture_diffuse1, TexCoords), L, E, normal_tangentspace);
    //~ if(texture_height1_passed == 0) FragColor = phongShading(texture(texture_diffuse1, TexCoords));
    //~ else FragColor = phongShadingBumpMapping(texture(texture_diffuse1, TexCoords), lightDirection_tangentspace, eyeDirection_tangentspace, normal_tangentspace, Normal);
    //FragColor = basicLight();
    //FragColor = basicTextured();
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
    FragColor = vec4(0.0);
    if(type == 0) FragColor = vec4(1.0);
    else if(type == 1) //only texture or diffuse
        if(texture_diffuse1_passed == 1)
            FragColor = FragColor = texture(texture_diffuse1, TexCoords);
        else
            FragColor = vec4(material.diffuse, 1.0);
    else if(type == 2)
        FragColor = basicLight();
            
    

} 


