//Light pass fragment shader
#version 330 core
out vec4 FragColor;
in vec2 aTexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

struct PointLight {
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct DirLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

#define NR_POINT_LIGHTS 4
uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];

uniform vec3 viewPos;

vec3 CalcDirLight(DirLight light, vec3 color, float spec, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 half_vec = normalize(-lightDir+viewDir);
    float specCalc = pow(clamp(dot(half_vec, normal), 0.0f, 1.0f), 256.0f);
    // combine results
    vec3 ambient  = light.ambient  * color;
    vec3 diffuse  = light.diffuse  * diff * color;
    vec3 specular = light.specular * specCalc * spec * color;
    return ambient*1.5+diffuse*2.5+specular;
}

vec3 CalcPointLight(PointLight light, vec3 color, float spec, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 half_vec = normalize(-lightDir+viewDir);
    float specCalc = pow(clamp(dot(half_vec, normal), 0.0f, 1.0f), 256.0f);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // combine results
    vec3 ambient = light.ambient * color;
    vec3 diffuse = light.diffuse * diff * color;
    vec3 specular = light.specular * specCalc * spec * color;
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient+diffuse)*0.3+specular;
}

void main()
{
    vec2 TexCoord = aTexCoords;
    vec3 Norm = texture(gNormal, TexCoord).xyz;
    vec3 FragPos = texture(gPosition, TexCoord).xyz;
    vec3 Color = texture(gAlbedoSpec, TexCoord).xyz;
    float spec = texture(gAlbedoSpec, TexCoord).w;
    vec3 viewDir = normalize(viewPos - FragPos);
	vec3 result = CalcDirLight(dirLight, Color, spec, Norm, viewDir);
	for(int i = 0; i < NR_POINT_LIGHTS; i++)
        result += CalcPointLight(pointLights[i], Color, spec, Norm, FragPos, viewDir);    

	FragColor = vec4(result, 1.0);
}