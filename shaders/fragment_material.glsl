#version 330 core

out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D texture_diffuse;
uniform sampler2D texture_specular;
uniform sampler2D texture_height;
uniform sampler2D texture_normal;

uniform bool has_diffuse = false, has_specular = false, has_normal = false, has_height = false; 

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
    int type;
};

in vec3 FragPos;  
in vec3 Normal;
in vec3 PosEyeSpace;
in vec3 lightDirection_tangentspace;
in vec3 eyeDirection_tangentspace;

  
uniform vec3 viewPos;
uniform Material material;
uniform Light light;

vec4 basicTextured() {
    vec3 ambient = light.ambient * material.ambient;
    vec3 diffuse = light.diffuse * (0.5 * material.diffuse);
    if(has_diffuse)
        diffuse *= texture(texture_diffuse, TexCoords).xyz;
    
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
    if(has_diffuse)
        return vec4(texColor.xyz + ambient + diffuse + specular, 1.0);
    else
        return vec4(ambient + diffuse + specular, 1.0);
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
    vec4 specmap = texture(texture_specular, TexCoords);
    vec3 specular = clamp(light.specular * material.specular * pow(max(dot(R, eyeDir), 0.0), material.shininess) * specmap.xyz, 0, 1);
    
    //return vec4(ambient + diffuse, 1) * texColor + vec4(specular, 1);
    return vec4(texColor.xyz + ambient + diffuse + specular, 1.0);
}

void main()
{
    
    //Starting bump map//////////////
        
    // Local normal, in tangent space. Expanding the range of the normal space
    vec3 tangentNormal;
    if(has_height)
        tangentNormal= normalize(texture2D(texture_height, TexCoords).rgb*2.0 - 1.0);
    else
        tangentNormal= normalize(Normal * 2.0 - 1.0);
        
    // Calculate the lighting diffuse value  */
    float diff = max(dot(tangentNormal, light.position), 0.0);
    vec3 diffuse = light.diffuse * (diff * material.diffuse);
    
    ////////////////////////////////
    
    ///////////////////////////////
    
    //Without bumpmapping
    vec3 Normal = normalize(Normal);
    vec3 L = normalize(light.position.xyz - PosEyeSpace); //light vector 
    vec3 E = normalize(-PosEyeSpace); // we are in Eye Coordinates, so EyePos is (0,0,0)
    
    //Bump mapping
    vec3 normal_tangentspace = normalize((texture( texture_height, TexCoords ).rgb-0.5) * 2.0);
    if(has_height){
        if(has_diffuse)
            FragColor = phongShading(texture(texture_diffuse, TexCoords));
        else {
            // ambient
            vec3 ambient = light.ambient * material.ambient;
            
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
            FragColor = phongShading(vec4(ambient + diffuse + specular, 1.0));
        }
    }
    else FragColor = phongShadingBumpMapping(texture(texture_diffuse, TexCoords), lightDirection_tangentspace, eyeDirection_tangentspace, normal_tangentspace, Normal);
    //////////////////////////////
    
    // ambient
    vec3 ambient = light.ambient * material.ambient;
  	
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    
    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * (spec * material.specular);  
        
    vec3 result = ambient + diffuse + specular;

        if(has_diffuse)
            FragColor = texture(texture_diffuse, TexCoords);
        else
            FragColor = vec4(ambient + diffuse + specular, 1.0);

            
    

} 


